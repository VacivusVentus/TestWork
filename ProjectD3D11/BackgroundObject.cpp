#include "BackgroundObject.h"
#include <Windows.h>
#include "resource.h"


#define RELEASE(p) if(p) {p->Release(); p = nullptr;}
BackgroundObject::BackgroundObject(ID3D11Device *m_device, ID3D11DeviceContext *context)
	: RenderObject(m_device, context)
	, m_vertexshader(nullptr)
	, m_pixelshader(nullptr)
	, m_constvalue(nullptr)
{
	memset(&valuesVS, 0, sizeof valuesVS);
	valuesVS.values[2] = -1.14;
	Initializate();
}

void BackgroundObject::Initializate()
{
	std::vector<unsigned char> data = getResourceFile(SHADER_V_BACKGROUND);
	HRESULT hr = m_device->CreateVertexShader(&data[0], data.size(), nullptr, &m_vertexshader);
	if (FAILED(hr))throw;
	data = getResourceFile(SHADER_F_BACKGROUND);
	hr = m_device->CreatePixelShader(&data[0], data.size(), nullptr, &m_pixelshader);
	if (FAILED(hr))throw;
	CD3D11_BUFFER_DESC m_const(sizeof valuesVS, D3D11_BIND_CONSTANT_BUFFER);
	hr = m_device->CreateBuffer(&m_const, nullptr, &m_constvalue);
	if (FAILED(hr))throw;
}

void BackgroundObject::draw()
{
	m_context->UpdateSubresource(m_constvalue, NULL, NULL, &valuesVS, 0, 0);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->VSSetShader(m_vertexshader, 0, 0);
	m_context->VSSetConstantBuffers(0, 1, &m_constvalue);
	m_context->PSSetShader(m_pixelshader, 0, 0);
	m_context->Draw(6, 0);
}

void BackgroundObject::deInitializate()
{
	RELEASE(m_constvalue);
	RELEASE(m_pixelshader);
	RELEASE(m_vertexshader);
}

void BackgroundObject::update(float tick)
{
	static float a = 1.0;
	valuesVS.values[2] += tick * a;
	if (abs(valuesVS.values[2]) > 10.0)
	{
		a = -a;
		if (valuesVS.values[2] > 0.0) valuesVS.values[2] = 10.;
		else valuesVS.values[2] = -10.;
	}
}

void BackgroundObject::setWindowSize(int width, int height)
{
	valuesVS.values[0] = width;
	valuesVS.values[1] = height;
}