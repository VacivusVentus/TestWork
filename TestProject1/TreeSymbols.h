#pragma once
#include<mutex>
#define TCHAR  char

class TreeSymbols
{
public:
	TreeSymbols(TCHAR ch);
	TreeSymbols() {}
	~TreeSymbols();
	bool operator == (TCHAR &ch) { return ch == symb; }
	TreeSymbols* findSymbol(TCHAR ch);
	void includeRepeatChainSymbols();
	//******************************************
	TCHAR getSymbol() const { return symb; }
	unsigned getCount() const { return count; }
	unsigned getRepeat() const { return repeat; }
	TreeSymbols **getInside() const { return childList; }
	void clearElems() { childList = nullptr; count = 0; }
private:
	std::mutex mutex;
	TCHAR symb;
	unsigned repeat;//повторение последовательности
	TreeSymbols **childList;//последующие символы
	unsigned amount, count;//общие зарезервированное кол-во последующих символо, текущие кол-во
};

