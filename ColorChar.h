#pragma once
class CColorChar
{
public:
	CColorChar(void);
	~CColorChar(void);

//member
public:
	bool _start;
	bool _blasted;
	bool _completed;

	int _row;
	int _col;
	char _myChar;

//method
public:
	void setColor(int color);				
	int colorByChar(char c);
};

