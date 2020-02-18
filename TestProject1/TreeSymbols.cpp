#include "TreeSymbols.h"
#include <memory.h>
#include <cctype>
#define SIZE_BLOCK 10

TreeSymbols::TreeSymbols(TCHAR ch) : symb(ch)
{
	repeat = 0;
}

TreeSymbols::~TreeSymbols()
{
	if (childList == nullptr) return;
	for (int i = 0; i < count; i++)
	{
		if (childList[i])
		{
			delete childList[i];
		}
	}
	delete[] childList;
	childList = nullptr;
}

TreeSymbols* TreeSymbols::findSymbol(TCHAR chx)
{
	TCHAR ch = std::tolower(chx);
	TreeSymbols* nextbranch = nullptr;
	mutex.lock();
	for (int i = 0; i < count; i++)
	{
		if (childList[i][0] == ch)
		{
			mutex.unlock();
			return childList[i];
		}
	}
	if (!amount)
	{
		childList = new TreeSymbols*[SIZE_BLOCK];
		if (!childList)
		{
			mutex.unlock();
			return nullptr;
		}
		amount = SIZE_BLOCK;
	}
	else if (amount == count)
	{
		unsigned clamount = amount + SIZE_BLOCK;
		TreeSymbols **cloud = new TreeSymbols*[clamount]; 
		if (!cloud)
		{
			mutex.unlock();
			return nullptr;
		}
		memcpy(cloud, childList, sizeof(TreeSymbols*) * amount);
		delete[] childList;
		childList = cloud;
	}
	TreeSymbols *cloud = new TreeSymbols(ch);
	if (!cloud)
	{
		mutex.unlock();
		return nullptr;
	}
	childList[count++] = cloud;
	mutex.unlock();
	return cloud;
}

void TreeSymbols::includeRepeatChainSymbols()
{
	mutex.lock();
	repeat++;
	mutex.unlock();
}