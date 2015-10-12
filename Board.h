#pragma once

#include <string>
#include <vector>
#include "ColorChar.h"

using namespace std;

class CBoard
{
public:
	CBoard();
	CBoard(CBoard* parent, bool bBLZ);
	virtual ~CBoard(void);

//member
private:
	int _boardSize;			
	int _startRow;			
	int _startCol;			
	int _movesCount;		
	int _blastedCount;		
	int _remainCount;		

	string _algorithmType;
	int _colorCount;

	CColorChar* _points;
	vector<CColorChar> _boundry;

//method
public:
	void	run(int boardSize);							
	bool	boardInit(int boardSize);
	void	ProcessBlasted(char curChar);	
	bool	addBoundry(int row, int col, char curChar);
	int		IsSameChar(int row, int col, char curChar);
	int		runByAlgorithm(string algorithm, bool bBLZ);
	char	getCharByAlgorithm(string algorithm);
	int		getCountByGreedy1Adjacent(char curChar);
	int		getCountByGreedy1All(char curChar);
	int		getCountByGreedy2Adjacent(char curChar, char nextChar);
	int		getCountByGreedy2All(char curChar, char nextChar);
	bool	IsExist(char curChar);
};

