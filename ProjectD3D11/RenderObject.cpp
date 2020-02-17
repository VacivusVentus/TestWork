#include "RenderObject.h"

RenderObject::RenderObject(ID3D11Device *m_device, ID3D11DeviceContext *context)
{
	this->m_device = m_device;
	this->m_context = context;
}

RenderObject::~RenderObject()
{
}

std::vector<unsigned char>RenderObject::getResourceFile(unsigned int res)
{
	std::vector<unsigned char>data;
	auto mod = GetModuleHandle(0);
	HRSRC rs = FindResource(mod, MAKEINTRESOURCE(res), RT_RCDATA);
	auto global = LoadResource(mod, rs);
	char *ldata = (char*)LockResource(global);
	int size = SizeofResource(mod, rs);
	data.resize(size);
	memcpy(&data[0], ldata, size);
	FreeResource(global);
	return data;
}