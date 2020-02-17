#pragma once
#include "RenderObject.h"
class BackgroundObject : public RenderObject
{
public:
	BackgroundObject(ID3D11Device *m_device, ID3D11DeviceContext *context);
	void draw() override;
	void update(float tick) override;
	void deInitializate() override;
	void setWindowSize(int width, int height) override;
protected:
	void Initializate() override;
private:
	ID3D11VertexShader *m_vertexshader;
	ID3D11PixelShader *m_pixelshader;
	ID3D11Buffer *m_constvalue;
	struct 
	{
		float values[4];
	} valuesVS;
};

