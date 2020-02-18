#pragma once
#include <fstream>
#include <future>
#include <thread>
#include "TreeSymbols.h"

#define PART_SIZE 1024 * 1024

struct PartInfo
{
	TCHAR *symbols;
	unsigned __int64 sizeofpart;
	PartInfo()
	{
		symbols = new TCHAR[PART_SIZE];
	}
	~PartInfo()
	{
		delete[] symbols;
	}
};
class LookAtTextpart
{
public:
	LookAtTextpart(std::ifstream &lpfile, std::ofstream &lpoutFile, TreeSymbols &lptree, unsigned threedamount);
	void greatWork();
	void start();
	/*************************************************/
	unsigned strLength() const { return maxlength; }
	/*******************/

	void readPart(PartInfo* pinfo);
	TreeSymbols *getTreeBranch();
	void printToFile(unsigned divch, TCHAR *chs, TreeSymbols *el);
protected:
private:
	std::ifstream &file;
	std::ofstream &outfile;
	TreeSymbols &tree;
	std::mutex mutexFile;
	std::mutex mutexStrL;
	unsigned thr_amount;
	std::future<void> threeds[50];
	unsigned __int64 fileSize;
	unsigned __int64 pos;
	unsigned maxlength;

};

