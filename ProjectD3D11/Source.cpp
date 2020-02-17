#include<Windows.h>
#include<windowsx.h>
#include "Rendere.h"
#include "BackgroundObject.h"
#include "ImageFrameObj.h"
#include "ScrollBarElement.h"
static Rendere *renderer = nullptr;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmd, int nShow)
{
	WNDCLASSW wcl = {};
	wcl.hInstance = hInstance;
	wcl.lpfnWndProc = WndProc;
	wcl.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
	wcl.style = CS_VREDRAW | CS_HREDRAW;
	wcl.lpszClassName = L"D3D11_Test";
	renderer = new Rendere();
	if (!renderer) return -1;
	if (!renderer->isCorrect())
	{
		delete renderer;
		renderer = nullptr;
		return -1;
	}
	RegisterClass(&wcl);
	HWND hWin = CreateWindow(wcl.lpszClassName, L"Scroll frames", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hWin, nShow);
	UpdateWindow(hWin);
	MSG msg = {};
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnregisterClass(wcl.lpszClassName, hInstance);
	
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wP, LPARAM lP)
{
	static UINT lastx, lasty;
	if (msg == WM_DESTROY)
	{
		if (renderer)
		{
			delete renderer;
			renderer = nullptr;
		}
		PostQuitMessage(0);
	}
	else if (msg == WM_SIZE)
	{
		bool p = renderer->resize(hWnd);
		if(!p)MessageBeep(MB_OK);
	}
	else if (msg == WM_MOUSEMOVE)
	{
		lastx = GET_X_LPARAM(lP);
		lasty = GET_Y_LPARAM(lP);
		renderer->setPositionCursor(lastx, lasty);
	}
	else if (msg == WM_MOUSELEAVE)
	{
		renderer->setPositionCursor(lastx, lasty);
	}
	else if (msg == WM_CREATE)
	{
		try
		{
			BackgroundObject *obj = new BackgroundObject(renderer->getDevice(), renderer->getDeviceContext());
			if (!obj) {
				throw;
			}
			renderer->addElement(obj);
			ImageFrameObj *imgobj = new ImageFrameObj(renderer->getDevice(), renderer->getDeviceContext());
			if (!imgobj) {
				throw;
			}
			renderer->addElement(imgobj);

			ScrollBarElement *scrollobj = new ScrollBarElement(renderer->getDevice(), renderer->getDeviceContext(), imgobj);
			if (!scrollobj) {
				throw;
			}
			renderer->addElement(scrollobj);
			renderer->addNewObjNeedingCurPos(imgobj);
			renderer->addNewObjNeedingCurPos(scrollobj);
		}
		catch (...)
		{
			DestroyWindow(hWnd);
			return 0;
		}
	}
	else return DefWindowProc(hWnd, msg, wP, lP);
	return 0;
}