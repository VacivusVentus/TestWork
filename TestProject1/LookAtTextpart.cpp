#include "LookAtTextpart.h"

const TCHAR *endstr = " \n,.!?@#$%^&*()=+-_\"'\\/:;";
LookAtTextpart::LookAtTextpart(std::ifstream &lpfile, TreeSymbols &lptree, unsigned threedamount)
	: file(lpfile)
	, tree(lptree)
	, thr_amount(threedamount)
	, threeds(nullptr)
{
	threeds = new std::thread*[thr_amount];
	if (!threeds)
	{
		throw;
	}
	file.seekg(0, std::ios::end);
	fileSize = file.tellg();
}

void LookAtTextpart::greatWork()
{
	PartInfo partInfo = {};
	readPart(&partInfo);

	TreeSymbols *symbSession = nullptr;
	for (;;)
	{
		readPart(&partInfo);
		if (partInfo.sizeofpart == 0) break;
		for (size_t i = 0; i < partInfo.sizeofpart; i++)
		{
			TCHAR ch[2] = { 0 };
			ch[0] = partInfo.symbols[i];
			if (strstr(endstr, ch) == NULL)
			{

			}
			else
			{
			}
		}
	}
}

void LookAtTextpart::start()
{
	unsigned corrThreed = 0;
	for (int i = 0; i < thr_amount; i++)
	{
		threeds[i] = new std::thread([=]() {this->greatWork(); });
	}

	for (int i = 0; i < thr_amount; i++)
	{
		if (threeds[i])
		{
			threeds[i]->join();
			delete[] threeds[i];
		}
	}
}

void LookAtTextpart::readPart(PartInfo* pinfo)
{
	mutexFile.lock();
	file.seekg(pos);
	unsigned __int64 untilEnd = fileSize - pos;
	unsigned __int64 nearEnd = PART_SIZE;
	if (untilEnd < nearEnd) nearEnd = untilEnd;
	file.read(pinfo->symbols, nearEnd);
	for (int i = nearEnd - 1; i >= 0; i--)
	{
		TCHAR ch[2] = { 0 };
		ch[0] = pinfo->symbols[i];
		if (strstr(endstr, ch)) break;
		else nearEnd;
	}
	if (nearEnd == 0)
	{
		mutexFile.unlock();
		pinfo->sizeofpart = 0;
		return;
	}
	pos += nearEnd;
	pinfo->sizeofpart = nearEnd;
	mutexFile.unlock();
}