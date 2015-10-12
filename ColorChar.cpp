#include <iostream>
#include <windows.h>
#include "ColorChar.h"

using namespace std;

CColorChar::CColorChar(void)
{
	_blasted = false;
	_start = false;
	_completed = false;
}

CColorChar::~CColorChar(void)
{
}

// set char color 
void CColorChar::setColor(int color)				
{
	HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hCon, color);
}

// get windows color corresponding to char
int CColorChar::colorByChar(char c)
{								
	switch (c)
	{
		case 'G':
			return 2;
		case 'N':
			return 10;
		case 'R':
			return 12;
		case 'B':
			return 9;
		case 'P':
			return 13;
		case 'Y':
			return 14;
		case 'C':
			return 3;
		case 'W':
			return 15;
		case 'M':
			return 5;
		case 'S':
			return 11;
	}
	return 7;
}
