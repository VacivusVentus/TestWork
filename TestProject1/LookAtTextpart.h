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
	LookAtTextpart(std::ifstream &lpfile, std::ofstream &lpoutFile, TreeSymbols &lptree, unsigned threedamount);
	void greatWork();
	void start();
	/*************************************************/
	unsigned strLength() const { return maxlength; }
protected:
	void readPart(PartInfo* pinfo);
	TreeSymbols *getTreeBranch();
	void printToFile(unsigned divch, TCHAR *chs, TreeSymbols *el);
private:
	std::ifstream &file;
	std::ofstream &outfile;
	TreeSymbols &tree;
	std::mutex mutexFile;
	std::mutex mutexStrL;
	unsigned thr_amount;
	std::thread **threeds;
	unsigned __int64 fileSize;
	unsigned __int64 pos;
	unsigned maxlength;

};

