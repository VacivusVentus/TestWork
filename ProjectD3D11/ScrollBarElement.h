#pragma once
#include "RenderObject.h"
class ScrollBarElement : public RenderObject
{
public:
	ScrollBarElement(ID3D11Device *m_device, ID3D11DeviceContext *context, RenderObject *imgFrame);
	void draw() override;
	void update(float tick) override;
	void deInitializate() override;
	void setWindowSize(int width, int height) override;
	void setPositionCursor(unsigned x, unsigned y) override;
protected:
	void Initializate() override;
private:
	ID3D11Buffer *m_constBuff;
	ID3D11VertexShader *m_vshader;
	ID3D11PixelShader *m_pshader;

	RenderObject *imgFrame;
	struct
	{
		float screen[4];
		unsigned int scrolls[4];
	}mainvalues;
};

