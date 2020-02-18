#include "Rendere.h"
#include <Windows.h>

#define RELEASE(p) if(p) {p->Release(); p = nullptr;}
#define PACH_OBJ 5

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "Winmm.lib")

static Rendere *currentRenderer = NULL;

Rendere::Rendere()
	: d_factory(nullptr)
	, m_device(nullptr)
	, m_context(nullptr)
	, m_swapchain(nullptr)
	, m_rendertarget(nullptr)
	, work_thread(nullptr)
	, m_samplestate(nullptr)
	, blendstate(nullptr)
	, dxgi_adapter(nullptr)
	, dxgi_device(nullptr)
{
	currentRenderer = this;
	memset(&needReadCursorPos, 0, sizeof needReadCursorPos);
	D3D_FEATURE_LEVEL f_levels[] = { D3D_FEATURE_LEVEL_11_0 };
	HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, f_levels, ARRAYSIZE(f_levels),
		D3D11_SDK_VERSION, &m_device, &level, &m_context);
	correctly = true;
	if (FAILED(hr))
	{
		correctly = false;
		return;
	}
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if (FAILED(m_device->CreateSamplerState(&sampDesc, &m_samplestate))) correctly = false;

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));
	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_ONE;
	rtbd.DestBlend = D3D11_BLEND_DEST_COLOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.RenderTarget[0] = rtbd;
	hr = m_device->CreateBlendState(&blendDesc, &blendstate);
	if (FAILED(hr))
	{
		correctly = false;
		return;
	}

	hr = m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device);
	if (FAILED(hr))
	{
		correctly = false;
		return;
	}
	hr = dxgi_device->GetAdapter(&dxgi_adapter);
	if (FAILED(hr))
	{
		correctly = false;
		return;
	}
	hr = dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&d_factory);
	if (FAILED(hr))
	{
		correctly = false;
		return;
	}
}

Rendere::~Rendere()
{
	stopThread();
	if (needReadCursorPos.objects)
	{
		delete[] needReadCursorPos.objects;
		memset(&needReadCursorPos, 0, sizeof needReadCursorPos);
	}
	size_t amount = listobj.size();
	for (int i = 0; i < amount; i++)
	{
		listobj[i]->deInitializate();
		delete listobj[i];
	}
	RELEASE(d_factory);
	RELEASE(dxgi_adapter);
	RELEASE(dxgi_device);
	RELEASE(blendstate);
	RELEASE(m_swapchain);
	RELEASE(m_samplestate);
	RELEASE(m_context);
	RELEASE(m_device);
}

bool Rendere::resize(HWND hWnd)
{
	d_factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
	stopThread();
	RECT r = {};
	GetClientRect(hWnd, &r);
	RELEASE(m_rendertarget);
	RELEASE(m_swapchain);
	DXGI_SWAP_CHAIN_DESC dxgi_sw = {};
	dxgi_sw.BufferCount = 1;
	dxgi_sw.OutputWindow = hWnd;
	dxgi_sw.Windowed = TRUE;
	dxgi_sw.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	dxgi_sw.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgi_sw.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	dxgi_sw.BufferDesc.Width = r.right - r.left;
	dxgi_sw.BufferDesc.Height = r.bottom - r.top;
	dxgi_sw.SampleDesc.Count = 1;
	HRESULT hr = d_factory->CreateSwapChain(m_device, &dxgi_sw, &m_swapchain);
	if (FAILED(hr))
	{
		MessageBox(hWnd, L"SwapChain", L"", MB_OK | MB_ICONERROR);
		return false;
	}
	ID3D11Texture2D *texture = nullptr;
	m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&texture);
	if (!texture)
	{
		return false;
	}
	hr = m_device->CreateRenderTargetView(texture, nullptr, &m_rendertarget);
	texture->Release();
	if (FAILED(hr))
	{
		return false;
	}
	m_context->OMSetRenderTargets(1, &m_rendertarget, nullptr); 
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	m_context->OMSetBlendState(blendstate, blendFactor, sampleMask);
	D3D11_VIEWPORT vp = {};
	vp.Width = r.right - r.left;
	vp.Height = r.bottom - r.top;
	vp.MaxDepth = 1.0f;	
	m_context->RSSetViewports(1, &vp);
	{
		int amount = listobj.size();
		RenderObject ** list = &listobj[0];
		for (int i = 0; i < amount; i++)
		{
			list[i]->setWindowSize(dxgi_sw.BufferDesc.Width, dxgi_sw.BufferDesc.Height);
		}
	}
	work_thread = new std::thread([=] {
		this->render();
		});
	if (!work_thread) return false;
	return true;
}

void Rendere::stopThread()
{
	if (work_thread)
	{
		renwork = false;
		work_thread->join();
		delete work_thread;
		work_thread = nullptr;
	}
}

void Rendere::addElement(RenderObject *object)
{
	listobj.push_back(object);
}

void Rendere::setPositionCursor(unsigned x, unsigned y)
{
	if (needReadCursorPos.blockedFlags == 0xFFFFFFFF) return;
	for (size_t i = 0; i < needReadCursorPos.count; i++)
	{
		needReadCursorPos.objects[i]->setPositionCursor(x, y);
	}
}

void Rendere::addNewObjNeedingCurPos(RenderObject *obj)
{
	if (needReadCursorPos.amount == 0)
	{
		needReadCursorPos.objects = new RenderObject*[PACH_OBJ];
		if (!needReadCursorPos.objects)
		{
			throw;
		}
		needReadCursorPos.amount = PACH_OBJ;
	}
	else if (needReadCursorPos.amount == needReadCursorPos.count)
	{
		RenderObject **cloud = new RenderObject*[needReadCursorPos.amount + PACH_OBJ];
		if (!cloud)
		{
			throw;
		}
		memcpy(cloud, needReadCursorPos.objects, sizeof(RenderObject*) * needReadCursorPos.amount);
		delete[] needReadCursorPos.objects;
		needReadCursorPos.objects = cloud;
		needReadCursorPos.amount += PACH_OBJ;
	}
	needReadCursorPos.objects[needReadCursorPos.count++] = obj;

}

Rendere *Rendere::getCurrentRenderer()
{
	return currentRenderer;
}

void Rendere::render()
{
	renwork = true;
	DWORD last = timeGetTime();
	bool isClicked = false;
	bool sendSignalClicked = false;
	while (renwork)
	{
		if (GetAsyncKeyState(VK_LBUTTON))
		{
			if (isClicked == false)
			{
				sendSignalClicked = true;
			}
			isClicked = true;
		}
		else
		{
			isClicked = false;
			needReadCursorPos.blockedFlags = 0;
		}
		DWORD current = timeGetTime();
		float tick = (current - last) * 0.0001f;
		float color[] = { 0.2, 0.2,0.2,1.0 };
		m_context->ClearRenderTargetView(m_rendertarget, color);
		//***********Render************************************
		{
			int amount = listobj.size();
			RenderObject ** list = &listobj[0];
			m_context->PSSetSamplers(0, 1, &m_samplestate);
			for (int i = 0; i < amount; i++)
			{
				if (sendSignalClicked)
				{
					list[i]->clickedObj();
				}
				list[i]->draw();
				list[i]->update(tick);
			}
		}
		sendSignalClicked = false;
		m_swapchain->Present(1, 0);
		last = current;
	}

	m_context->OMSetBlendState(NULL, 0, 0);
}