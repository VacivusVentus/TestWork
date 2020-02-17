#pragma once
#include "RenderObject.h"
class ImageFrameObj : public RenderObject
{
public:
	ImageFrameObj(ID3D11Device *m_device, ID3D11DeviceContext *context);
	void draw() override;
	void update(float tick) override;
	void deInitializate() override;
	void setWindowSize(int width, int height) override;
	void setIdDivImgs(char iddiv);
	void setPositionCursor(unsigned x, unsigned y) override;
	void clickedObj() override;
	//***********************
protected:
	void Initializate() override;
	void updImgIDs();
private:
	ID3D11Texture2D *texture;
	ID3D11ShaderResourceView *tex_viewer;
	ID3D11VertexShader *m_vertexshader;
	ID3D11PixelShader *m_pixelshader;
	ID3D11Buffer *m_genBuff;
	ID3D11Buffer *m_imgInfoBuff;
	struct 
	{
		unsigned int idImage[4 * 5];
	} settingMap;
	struct 
	{
		float values[4];
	}valuesVS;
	bool isEnterFrame;
	float dSpeedX, dSpeedY;
	float speedX, speedY;
	UINT32 arrayImg[14][6];
	UINT iX, iY;
	float xf, yf;
	bool firstcoord;
};

