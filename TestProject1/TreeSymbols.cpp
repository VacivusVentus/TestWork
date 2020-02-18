#include "TreeSymbols.h"
#include <memory.h>
#include <cctype>

#define SIZE_BLOCK 10
#define SIZE_GBLOCK 100

static TreeSymbols*** allTrees = nullptr;
static TreeSymbols *mainTree;
static unsigned globalcount = 0;
static unsigned globalpart = 0;

TreeSymbols::TreeSymbols(TCHAR ch)  
	: symb(ch)
	, childList(nullptr)
	, count(0)
	, amount(0)
{
	repeat = 0;
}

TreeSymbols::~TreeSymbols()
{
	if (childList == nullptr) return;
	
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
		amount = clamount;
		childList = cloud;
	}
	TreeSymbols *cloud = new TreeSymbols(ch);
	if (!cloud)
	{
		mutex.unlock();
		return nullptr;
	}
	//TreeSymbols::addTreetAll(cloud);
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

void TreeSymbols::clearSimple()
{
	count = 0;
	amount = 0;
	if (childList)
	{
		delete[] childList;
		childList = nullptr;
	}
}
/***********************/
void TreeSymbols::initTreeList()
{
	allTrees = new TreeSymbols**[1];
	if (allTrees)
	{
		globalcount = 0;
		globalpart = 0;
		allTrees[globalpart] = new TreeSymbols*[SIZE_GBLOCK];
	}
}
void TreeSymbols::setMainTree(TreeSymbols *lpmainTree)
{
	mainTree = lpmainTree;
}

void TreeSymbols::addTreetAll(TreeSymbols *lpmainTree)
{
	if (globalcount == SIZE_GBLOCK)
	{
		TreeSymbols ***cloud = new TreeSymbols**[globalpart + 2];
		if (!cloud) exit(1);
		memcpy(cloud, allTrees, sizeof(TreeSymbols**) * (globalpart + 1));
		globalpart++;
		TreeSymbols **branch = (TreeSymbols **)malloc(SIZE_GBLOCK * sizeof(TreeSymbols*));
		cloud[globalpart] = branch;
		if (!cloud[globalpart]) exit(1);
		globalcount = 0;
	}
	auto uuu = allTrees[globalpart];
	allTrees[globalpart][globalcount++] = lpmainTree;
}

void TreeSymbols::release()
{
	for (int i = 0; i <= globalpart; i++)
	{
		if (allTrees[globalpart])
		{
			int amount = (i < globalpart) ? SIZE_GBLOCK : globalcount;
			for (int j = 0; j < amount; j++)
			{
				delete allTrees[globalpart][j];
			}
			free(allTrees[globalpart]);
		}
	}
	delete[] allTrees;
}