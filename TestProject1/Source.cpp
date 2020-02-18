#include <iostream>
#include <Windows.h>
#include "LookAtTextpart.h"


int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Bit of an argument!\nExample EXE /inp:filename.txt /out:tale.txt");
	}	

	//******************************************************************************
	TreeSymbols symp;
	TreeSymbols::setMainTree(&symp);
	//*********************************************************************************
	SYSTEM_INFO sysinf;
	GetSystemInfo(&sysinf);
	//*********************************************************************************
	std::ifstream infile;
	std::ofstream outfile;
	char flagcorrectargs = 0;
	for (int i = 1; i < argc; i++)
	{
		if (flagcorrectargs == 3) break;
		char *infn = strstr(argv[i], "inp:");
		char *outfn = strstr(argv[i], "out:");
		if (infn)
		{
			char *filename = &argv[i][5];
			infile.open(filename, std::ios::binary);
			if (infile.is_open())
			{
				flagcorrectargs |= 1;
			}
		}
		else if (outfn)
		{
			outfile.open(&argv[i][5], std::ios::binary);
			if (outfile.is_open())
			{
				flagcorrectargs |= 2;
			}
		}
	}
	if (flagcorrectargs != 3) return -1;
	LookAtTextpart *looker = nullptr;
	try
	{
		looker = new LookAtTextpart(infile, outfile, symp, sysinf.dwNumberOfProcessors);
	}
	catch (...)
	{
		if (looker)
		{
			delete looker;
		}
		infile.close();
		outfile.close();
		return -1;
	}
	TreeSymbols::initTreeList();
	looker->start();
	if (looker)
	{
		delete looker;
	}
	infile.close();
	outfile.close();
	//*********************************************************************************
	symp.release();
	return 0;
}