#pragma once
#include <d3d11.h>
#include <vector>
class RenderObject
{
public:
	RenderObject(ID3D11Device *m_device, ID3D11DeviceContext *context);
	~RenderObject();
	virtual void draw() = NULL;
	virtual void update(float tick) = NULL;
	virtual void deInitializate() = NULL; 
	virtual void setWindowSize(int width, int height) = NULL;
	virtual void setPositionCursor(unsigned x, unsigned y) {}
	virtual void clickedObj() {}
protected:
	virtual void Initializate() = NULL;
	std::vector<unsigned char> getResourceFile(unsigned int res);
protected:
	ID3D11Device *m_device;
	ID3D11DeviceContext *m_context;
};

