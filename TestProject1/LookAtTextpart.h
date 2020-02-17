#pragma once
#include <fstream>
#include <future>
#include <thread>
#include "TreeSymbols.h"

#define PART_SIZE 4096

struct PartInfo
{
	TCHAR symbols[PART_SIZE];
	unsigned int sizeofpart;
};
class LookAtTextpart
{
public:
	LookAtTextpart(std::ifstream &lpfile, TreeSymbols &lptree, unsigned threedamount);
	void greatWork();
	void start();
protected:
	void readPart(PartInfo* pinfo);
private:
	std::ifstream &file;
	TreeSymbols &tree;
	std::mutex mutexFile;
	unsigned thr_amount;
	std::thread **threeds;
	unsigned __int64 fileSize;
	unsigned __int64 pos;

};

