#include "LookAtTextpart.h"

const TCHAR *endstr = " \n,.!?@#$%^&*()=+-_\"'\\/:;";
#define MX(a, b) (a > b) ? a : b

LookAtTextpart::LookAtTextpart(std::ifstream &lpfile, std::ofstream &lpoutFile, TreeSymbols &lptree, unsigned threedamount)
	: file(lpfile)
	, outfile(lpoutFile)
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
		unsigned lenght = 0;
		readPart(&partInfo);
		if (partInfo.sizeofpart == 0) break;
		for (size_t i = 0; i < partInfo.sizeofpart; i++)
		{
			TCHAR ch[2] = { 0 };
			ch[0] = partInfo.symbols[i];
			if (strstr(endstr, ch))
			{
				if (symbSession)
				{
					symbSession->includeRepeatChainSymbols();
					symbSession = nullptr;
					if (lenght > maxlength)
					{
						mutexStrL.lock();
						maxlength = MX(maxlength, lenght); // Мог изменить другой поток
						mutexStrL.unlock();
					}
					lenght = 0;
				}
			}
			else
			{
				if (!symbSession)symbSession = tree.findSymbol(ch[0]);
				else symbSession = symbSession->findSymbol(ch[0]);
				lenght++;
			}
		}
	}
}

void LookAtTextpart::start()
{
	maxlength = 0;
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
			threeds[i] = nullptr;
		}
	}
	//Save to file
	pos = 0;
	for (int i = 0; i < thr_amount; i++)
	{
		threeds[i] = new std::thread([=]() {
			TCHAR *word = new TCHAR[maxlength + 1];
			if (!word) return;
			for (TreeSymbols *entry = this->getTreeBranch(); entry; entry = this->getTreeBranch())
			{
				this->printToFile(0, word, entry);
			}
			delete[] word;
			});
	}

	for (int i = 0; i < thr_amount; i++)
	{
		if (threeds[i])
		{
			threeds[i]->join();
			delete[] threeds[i];
			threeds[i] = nullptr;
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

TreeSymbols *LookAtTextpart::getTreeBranch()
{
	mutexStrL.lock();
	if (pos < tree.getCount())
	{
		TreeSymbols *send = (tree.getInside())[pos++];
		mutexStrL.unlock();
		return send;
	}
	mutexStrL.unlock();
	return nullptr;
}

void LookAtTextpart::printToFile(unsigned divch, TCHAR *chs, TreeSymbols *el)
{
	chs[divch] = el->getSymbol();
	unsigned r = el->getRepeat();
	if (r)
	{
		TCHAR amtxt[16] = { 0 };
		sprintf_s(amtxt, sizeof amtxt, " = %d\n", r);
		mutexFile.lock();
		outfile.write(chs, divch + 1);
		outfile.write(amtxt, strlen(amtxt));
		mutexFile.unlock();
	}
	unsigned amount = el->getCount();
	if (amount)
	{
		TreeSymbols **child = el->getInside();
		for (int i = 0; i < amount; i++)
		{
			printToFile(divch + 1, chs, child[i]);
		}
	}
}