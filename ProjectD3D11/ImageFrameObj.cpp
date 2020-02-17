#include "ImageFrameObj.h"
#include "resource.h"
#include <lodepng.h>
#include <vector>
#include "Rendere.h"

#define MIN(a,b) (a < b) ? a : b
#define MAX(a,b) (a > b) ? a : b

#define RELEASE(p) if(p) {p->Release(); p = nullptr;}
ImageFrameObj::ImageFrameObj(ID3D11Device *m_device, ID3D11DeviceContext *context)
	: RenderObject(m_device, context)
	, texture(nullptr)
	, tex_viewer(nullptr)
	, m_vertexshader(nullptr)
	, m_pixelshader(nullptr)
	, m_genBuff(nullptr)
	, m_imgInfoBuff(nullptr)
{

	firstcoord = false;
	memset(&settingMap, 0, sizeof settingMap);
	memset(&valuesVS, 0, sizeof valuesVS);
	dSpeedX = 0.0;
	dSpeedY = 0.0;
	speedX = 0.0;
	speedY = 0.0;
	UINT pId = 0;
	for (size_t i = 0; i < 14; i++)
	{
		for (size_t j = 0; j < 6; j++)
		{
			arrayImg[i][j] = pId;
			pId = (pId + 1) % (14 * 4);
		}

	}
	iX = 0;
	iY = 0;
	Initializate();
	updImgIDs();
}

void ImageFrameObj::draw()
{
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->VSSetShader(m_vertexshader, 0, 0);
	m_context->PSSetShader(m_pixelshader, 0, 0);
	m_context->PSSetShaderResources(0, 1, &tex_viewer);

	m_context->VSSetConstantBuffers(0, 1, &m_genBuff);
	m_context->UpdateSubresource(m_genBuff, 0, NULL, &valuesVS, 0, 0);

	m_context->VSSetConstantBuffers(1, 1, &m_imgInfoBuff);
	m_context->UpdateSubresource(m_imgInfoBuff, 0, NULL, &settingMap, 0, 0);

	m_context->Draw(6 * 20, 0);
}
void ImageFrameObj::update(float tickx)
{
	if (BLOCK_SEL_CONTROL)
	{
		float tick = tickx * 0.3;
		if (dSpeedX == 0.0)
		{
			if (speedX > 0.0)
			{
				speedX -= tick;
				if (speedX < 0.0) speedX = 0.0;
			}
			else if (speedX < 0.0)
			{
				speedX += tick;
				if (speedX > 0.0) speedX = 0.0;
			}
		}
		else
		{
			if (dSpeedX > 0.0)
			{
				speedX = MIN(speedX + tick, 0.8);
			}
			if (dSpeedX < 0.0)
			{
				speedX = MAX(speedX - tick, -0.8);
			}
		}


		if (dSpeedY == 0.0)
		{
			if (speedY > 0.0)
			{
				speedY -= tick;
				if (speedY < 0.0) speedY = 0.0;
			}
			else if (speedY < 0.0)
			{
				speedY += tick;
				if (speedY > 0.0) speedY = 0.0;
			}
		}
		else
		{
			if (dSpeedY > 0.0)
			{
				speedY = MIN(speedY + tick, 0.8);
			}
			if (dSpeedY < 0.0)
			{
				speedY = MAX(speedY - tick, -0.8);
			}
		}
		valuesVS.values[2] += speedX;
		valuesVS.values[3] += speedY;
	}
	/************************************************************/
	bool chg = false;
	while (abs(valuesVS.values[2]) > 0.21)
	{
		if (valuesVS.values[2] > 0.0)
		{
			valuesVS.values[2] = MAX(0, valuesVS.values[2] - 0.21);
			iX--;
		}
		else
		{
			valuesVS.values[2] = MIN(0, valuesVS.values[3] + 0.21);
			iX++;
			iX %= 14;
		}
		chg = true;

		if (iX < 0)  iX += 14;
	}

	while (abs(valuesVS.values[3]) > 0.31)
	{
		if (valuesVS.values[3] > 0.0)
		{
			valuesVS.values[3] = MAX(0, valuesVS.values[3] - 0.31);
			iY--;
		}
		else
		{
			valuesVS.values[3] = MIN(0, valuesVS.values[3] + 0.31);

			iY++;
			iY %= 6;
		}
		chg = true;
		if (iY < 0)  iY += 6;
	}

	dSpeedX = 0.0;
	dSpeedY = 0.0;
	updImgIDs();
}

void ImageFrameObj::Initializate()
{
	std::vector<unsigned char> lp = RenderObject::getResourceFile(PICTURE_CARDS);
	std::vector<unsigned char> img;
	unsigned w, h;
	lodepng::decode(img, w, h, lp);
	D3D11_TEXTURE2D_DESC tex2Ddesc = {};
	tex2Ddesc.ArraySize = 1;
	tex2Ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tex2Ddesc.Height = h;
	tex2Ddesc.Width = w;
	tex2Ddesc.MipLevels = 1;
	tex2Ddesc.SampleDesc.Count = 1;
	tex2Ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	D3D11_SUBRESOURCE_DATA subdata = {};
	subdata.pSysMem = &img[0];
	subdata.SysMemPitch = w << 2;
	subdata.SysMemSlicePitch = w * h << 2;
	if (FAILED(m_device->CreateTexture2D(&tex2Ddesc, &subdata, &texture))) throw;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
	srvd.Format = tex2Ddesc.Format;
	srvd.Texture2D.MipLevels = 1;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	if (FAILED(m_device->CreateShaderResourceView(texture, &srvd, &tex_viewer))) throw;

	std::vector<unsigned char> data = getResourceFile(SHADER_V_FRAMEIMG);
	HRESULT hr = m_device->CreateVertexShader(&data[0], data.size(), nullptr, &m_vertexshader);
	if (FAILED(hr))throw;
	data = getResourceFile(SHADER_F_FRAMEIMG);
	hr = m_device->CreatePixelShader(&data[0], data.size(), nullptr, &m_pixelshader);
	if (FAILED(hr))throw;
	{
		CD3D11_BUFFER_DESC bufdesc(sizeof valuesVS, D3D11_BIND_CONSTANT_BUFFER);
		hr = m_device->CreateBuffer(&bufdesc, nullptr, &m_genBuff);
		if (FAILED(hr))throw;
	}
	{
		CD3D11_BUFFER_DESC bufdesc(sizeof settingMap, D3D11_BIND_CONSTANT_BUFFER);
		hr = m_device->CreateBuffer(&bufdesc, nullptr, &m_imgInfoBuff);
		if (FAILED(hr))throw;
		
	}
}
void ImageFrameObj::deInitializate()
{
	RELEASE(m_imgInfoBuff);
	RELEASE(m_genBuff);
	RELEASE(m_pixelshader);
	RELEASE(m_vertexshader);
	RELEASE(tex_viewer);
	RELEASE(texture);
}

void ImageFrameObj::setWindowSize(int width, int height)
{
	valuesVS.values[0] = width;
	valuesVS.values[1] = height;
}

void ImageFrameObj::setIdDivImgs(char iddiv)
{
	if (iddiv == 0)
	{
		dSpeedY = 1.0f;
	}
	else if (iddiv == 1)
	{
		dSpeedY -= 1.0f;
	}
	if (iddiv == 2)
	{

		dSpeedX -= 1.0f;
	}
	else if (iddiv == 3)
	{

		dSpeedX += 1.0f;
	}
}

void ImageFrameObj::setPositionCursor(unsigned x, unsigned y)
{
	if (Rendere::getCurrentRenderer()->getBolockFlags() & BLOCK_SEL_DRAG) return;
	float min = ((valuesVS.values[0] < valuesVS.values[1]) ? valuesVS.values[0] : valuesVS.values[1]) / 2.0;
	float nX = (x - (valuesVS.values[0] / 2.0)) / min;
	float nY = (y - (valuesVS.values[1] / 2.0)) / min;
	if (!GetAsyncKeyState(VK_LBUTTON) && firstcoord)
	{
		firstcoord = false;
		return;
	}
	if (firstcoord)
	{
		isEnterFrame = (abs(nX) < 0.315 && abs(nY) < 0.17);
		speedX = 0.0;
		speedY = 0.0;
		xf = nX;
		yf = nY;
		firstcoord = false;
	}
	else if(isEnterFrame)
	{
		if (GetAsyncKeyState(VK_LBUTTON))
		{
			valuesVS.values[2] += nX - xf;
			valuesVS.values[3] += yf - nY;
			xf = nX;
			yf = nY;
		}
		else
		{

			speedX = nX - xf;
			speedY = yf - nY;
			isEnterFrame = false;
		}
	}
}

void ImageFrameObj::updImgIDs()
{

	int p = 0;
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 5; i++)
		{
			settingMap.idImage[p++] = arrayImg[(iX + i) % 14][(iY + j) % 6];
		}
	}
}

void ImageFrameObj::clickedObj()
{
	firstcoord = true;
} 