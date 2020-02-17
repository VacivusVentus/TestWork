#pragma once
#include <d3d11.h>
#include<dxgi.h>
#include <DirectXMath.h>
#include <thread>
#include "RenderObject.h"


#define BLOCK_SEL_CONTROL (1 << 0)
#define BLOCK_SEL_DRAG    (1 << 1)
#define BLOCK_ALL         0xFFFFFFFF

using namespace DirectX;


class Rendere
{
public:
	Rendere();
	~Rendere();
	bool resize(HWND);
	void addElement(RenderObject *object);
	void setPositionCursor(unsigned x, unsigned y);
	void addNewObjNeedingCurPos(RenderObject *obj);
	void render();
	static Rendere *getCurrentRenderer();
public:
	ID3D11Device *getDevice() const { return m_device; }
	ID3D11DeviceContext *getDeviceContext() const { return m_context; }
	bool isCorrect() const { return correctly; }
	unsigned getBolockFlags() const { return needReadCursorPos.blockedFlags; }
private:
	IDXGIFactory *d_factory;
	IDXGIDevice *dxgi_device;
	IDXGIAdapter *dxgi_adapter;
	ID3D11Device *m_device;
	ID3D11DeviceContext *m_context;
	IDXGISwapChain *m_swapchain;
	D3D_FEATURE_LEVEL level;
	ID3D11RenderTargetView *m_rendertarget;
	ID3D11BlendState *blendstate;
	std::thread *work_thread;
	bool correctly;
	bool renwork;
	std::vector<RenderObject*>listobj;
	ID3D11SamplerState *m_samplestate;
	struct 
	{
		RenderObject **objects;
		unsigned amount, count;
		unsigned blockedFlags;
	}needReadCursorPos;
protected:
	void stopThread();
};

