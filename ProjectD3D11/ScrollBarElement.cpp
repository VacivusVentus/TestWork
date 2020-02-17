#include "ScrollBarElement.h"
#include "ImageFrameObj.h"
#include "resource.h"
#include "Rendere.h"

#define RELEASE(p) if(p) {p->Release(); p = nullptr;}
ScrollBarElement::ScrollBarElement(ID3D11Device *m_device, ID3D11DeviceContext *context, RenderObject *imgFrame)
	:RenderObject(m_device, context)
	, m_constBuff(nullptr)
	, m_vshader(nullptr)
	, m_pshader(nullptr)
{
	memset(&mainvalues, 0, sizeof mainvalues);
	this->imgFrame = imgFrame;
	Initializate();
}

void ScrollBarElement::draw()
{
	m_context->VSSetShader(m_vshader, 0, 0);
	m_context->PSSetShader(m_pshader, 0, 0);
	m_context->VSSetConstantBuffers(0, 1, &m_constBuff);
	m_context->UpdateSubresource(m_constBuff, 0, nullptr, &mainvalues, 0, 0);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->Draw(12, 0);
}
void ScrollBarElement::update(float tick)
{
	if (Rendere::getCurrentRenderer()->getBolockFlags() & BLOCK_SEL_CONTROL) return;
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		for (int i = 0; i < 4; i++)
		{
			if (mainvalues.scrolls[i] == 1)
			{
				((ImageFrameObj*)imgFrame)->setIdDivImgs(i);
				return;
			}
		}
	}
}
void ScrollBarElement::deInitializate()
{
	RELEASE(m_pshader);
	RELEASE(m_vshader);
	RELEASE(m_constBuff);
}
void ScrollBarElement::setWindowSize(int width, int height)
{
	mainvalues.screen[0] = width;
	mainvalues.screen[1] = height;
}

void ScrollBarElement::Initializate()
{
	CD3D11_BUFFER_DESC cdesc(sizeof mainvalues, D3D11_BIND_CONSTANT_BUFFER);
	if (FAILED(m_device->CreateBuffer(&cdesc, nullptr, &m_constBuff)))
	{
		throw;
	}
	std::vector<unsigned char> data = RenderObject::getResourceFile(SHADER_V_SCROLL);
	if(FAILED(m_device->CreateVertexShader(&data[0], data.size(), nullptr, &m_vshader))) throw;
	data = RenderObject::getResourceFile(SHADER_F_SCROLL);
	if (FAILED(m_device->CreatePixelShader(&data[0], data.size(), nullptr, &m_pshader))) throw;
}

void ScrollBarElement::setPositionCursor(unsigned x, unsigned y)
{
	if (Rendere::getCurrentRenderer()->getBolockFlags() & BLOCK_SEL_CONTROL) return;
	float min = ((mainvalues.screen[0] < mainvalues.screen[1]) ? mainvalues.screen[0] : mainvalues.screen[1]) / 2.0;
	float nX = (x - (mainvalues.screen[0] / 2.0)) / min;
	float nY = (y - (mainvalues.screen[1] / 2.0)) / min;
	memset(mainvalues.scrolls, 0, sizeof mainvalues.scrolls);
	if (abs(nX) < 0.15)
	{
		if (nY > 0.34 && nY < 0.39)
		{
			mainvalues.scrolls[1] = 1;
		}

		if (nY < -0.34 && nY > -0.39)
		{
			mainvalues.scrolls[0] = 1;
		}
	}
	else if (abs(nY) < 0.15)
	{
		if (nX > 0.34 && nX < 0.42)
		{
			mainvalues.scrolls[3] = 1;
		}
		if (nX < -0.34 && nX > -0.42)
		{
			mainvalues.scrolls[2] = 1;
		}
	}
}