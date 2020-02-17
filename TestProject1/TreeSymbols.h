#pragma once

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
	unsigned getCount() const { return count; }
	TreeSymbols **getInside() const { return childList; }
	void clearElems() { childList = nullptr; count = 0; }
private:
	TCHAR symb;
	unsigned repeat;//���������� ������������������
	TreeSymbols **childList;//����������� �������
	unsigned amount, count;//����� ����������������� ���-�� ����������� �������, ������� ���-��
};

