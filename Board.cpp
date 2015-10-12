#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include "Board.h"

// Green, Neon, Red, Blue, Purple, Yellow, Cyan, White, Magenta, SkyBlue
const char ColorChars[] = { 'G', 'N', 'R', 'B', 'P', 'Y', 'C', 'W', 'M', 'S' };

// create board with boardSize
CBoard::CBoard()
{
}

// create the same board as parent 
CBoard::CBoard(CBoard* parent, bool bBLZ)
{
	_boardSize = parent->_boardSize;
	_colorCount = parent->_colorCount;
	_points = new CColorChar[_boardSize * _boardSize];

	// copy char and blasted status
	for (int row = 0; row < _boardSize; row++)
	{
		for (int col = 0; col < _boardSize; col++)
		{
			_points[row * _boardSize + col]._row = row;
			_points[row * _boardSize + col]._col = col;
			_points[row * _boardSize + col]._myChar = parent->_points[row * _boardSize + col]._myChar;
		}
	}

	_movesCount = 0;

	if(bBLZ)
	{
		_startRow = _boardSize / 2 + rand() % (_boardSize / 2);
		if(_startRow < _boardSize * 3 / 4) _startCol =  rand() % (_boardSize / 4);
		else  _startCol = rand() % (_boardSize / 2);
	}
	else
	{
		_startRow = rand() % _boardSize;
		if(_startRow < _boardSize / 2) _startCol =  rand() % _boardSize;
		if(_startRow < _boardSize * 3 / 4) _startCol =  _boardSize / 4 + rand() % (_boardSize * 3 / 4);
		else  _startCol = _boardSize / 2 + rand() % (_boardSize / 2);
	}
	_points[_startRow * _boardSize + _startCol]._start = true;

	// start point is first blasted
	_points[_startRow * _boardSize + _startCol]._blasted = true;
	_blastedCount = 1;

	// there is only start point first in boundry
	_boundry.push_back(_points[_startRow * _boardSize + _startCol]);

	// remain count is (total count - blasted count).
	_remainCount = _boardSize * _boardSize - _blastedCount;
}

CBoard::~CBoard(void)
{
	_boundry.clear();

	// delete points in memory
	delete[] _points;
}

// init board from file
bool CBoard::boardInit(int boarderSize)
{
	_boardSize = boarderSize;
	_colorCount = 10;
	_points = new CColorChar[_boardSize * _boardSize];

	// assign random chars in board with size of boardSize * boardSize
	for (int row = 0; row < _boardSize; row++)
	{
		for (int col = 0; col < _boardSize; col++)
		{
			_points[row * _boardSize + col]._row = row;
			_points[row * _boardSize + col]._col = col;
			_points[row * _boardSize + col]._myChar = ColorChars[rand() % 10];
		}
	}

	return true;
}

// cell with same color adjacent to the blasted region is added in boundry
bool CBoard::addBoundry(int row, int col, char curChar)
{
	if (row < 0 || row > _boardSize - 1 || col < 0 || col > _boardSize - 1) return true;
	int index = row * _boardSize + col;

	// add only cell with same color but no blasted
	if (_points[index]._blasted) return true;

	if (_points[index]._myChar == curChar)
	{
		_points[index]._blasted = true;

		// add only cell with same color but no blasted
		_boundry.push_back(_points[index]);

		// increase blast Count and also decrease reamin count when blasted 
		_blastedCount++;
		_remainCount--;

		return true;
	}

	return false;
}

// whether is the same color as selected color
int CBoard::IsSameChar(int row, int col, char curChar)
{
	if (row < 0 || row > _boardSize - 1 || col < 0 || col > _boardSize - 1) return 0;
	int index = row * _boardSize + col;

	// should be only new cell with same color but not blasted and computed
	// completed is use only for estimate
	if (!_points[index]._blasted && !_points[index]._completed &&
		_points[index]._myChar == curChar)
	{
		_points[index]._completed = true;
		return 1;
	}
	return 0;
}

// add new cell in and erase flooded cell from boundry
void CBoard::ProcessBlasted(char curChar)
{
	int i = 0;
	while (i < _boundry.size())
	{
		int row = _boundry[i]._row;
		int col = _boundry[i]._col;

		// add new cell with the same color in boundry
		bool left = addBoundry(row, col - 1, curChar);
		bool right = addBoundry(row, col + 1, curChar);
		bool top = addBoundry(row - 1, col, curChar);
		bool bottom = addBoundry(row + 1, col, curChar);

		// erase from boundry if all cell next to (left, right, top, bottom) are blasted
		if (left && right && top && bottom)
		{
			_boundry.erase(_boundry.begin() + i);
		}
		else i++;
	}
}

// get count of characters with curChar for greedy algorithm
int CBoard::getCountByGreedy1Adjacent(char curChar)
{
	vector<CColorChar> completedPoints;

	// add all cells next to boundry with the same color into vector 
	for (int i = 0; i < _boundry.size(); i++)
	{
		int row = _boundry[i]._row;
		int col = _boundry[i]._col;

		// add if is the same color as selected color
		if (IsSameChar(row, col - 1, curChar) == 1)
		{
			int index = row * _boardSize + col - 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row, col + 1, curChar) == 1)
		{
			int index = row * _boardSize + col + 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row - 1, col, curChar) == 1)
		{
			int index = (row - 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row + 1, col, curChar) == 1)
		{
			int index = (row + 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
	}

	// count to be added is count of vector
	int total = completedPoints.size();

	// after computed, completed status is clear for later use
	for (int i = 0; i < completedPoints.size(); i++)
	{
		int row = completedPoints[i]._row;
		int col = completedPoints[i]._col;
		_points[row * _boardSize + col]._completed = false;
	}

	completedPoints.clear();
	return total;
}
// get count of characters with curChar for greedyDeep algorithm

int CBoard::getCountByGreedy1All(char curChar)
{
	vector<CColorChar> completedPoints;
	for (int i = 0; i < _boundry.size(); i++)
	{
		int row = _boundry[i]._row;
		int col = _boundry[i]._col;

		// add if is the same color as selected color
		if (IsSameChar(row, col - 1, curChar) == 1)
		{
			int index = row * _boardSize + col - 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row, col + 1, curChar) == 1)
		{
			int index = row * _boardSize + col + 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row - 1, col, curChar) == 1)
		{
			int index = (row - 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row + 1, col, curChar) == 1)
		{
			int index = (row + 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
	}

	// consider newly added cell as well
	for (int i = 0; i < completedPoints.size(); i++)
	{
		int row = completedPoints[i]._row;
		int col = completedPoints[i]._col;

		// add if is the same color as selected color
		if (IsSameChar(row, col - 1, curChar) == 1)
		{
			int index = row * _boardSize + col - 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row, col + 1, curChar) == 1)
		{
			int index = row * _boardSize + col + 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row - 1, col, curChar) == 1)
		{
			int index = (row - 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row + 1, col, curChar) == 1)
		{
			int index = (row + 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
	}

	int total = completedPoints.size();

	// after computed, completed status is clear for later use
	for (int i = 0; i < completedPoints.size(); i++)
	{
		int row = completedPoints[i]._row;
		int col = completedPoints[i]._col;
		_points[row * _boardSize + col]._completed = false;
	}

	completedPoints.clear();
	return total;
}

// get count of characters with curChar for greedy2Deep algorithm
int CBoard::getCountByGreedy2Adjacent(char curChar, char nextChar)
{
	vector<CColorChar> completedPoints;
	for (int i = 0; i < _boundry.size(); i++)
	{
		int row = _boundry[i]._row;
		int col = _boundry[i]._col;

		// add if is the same color as selected color
		if (IsSameChar(row, col - 1, curChar) == 1)
		{
			int index = row * _boardSize + col - 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row, col + 1, curChar) == 1)
		{
			int index = row * _boardSize + col + 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row - 1, col, curChar) == 1)
		{
			int index = (row - 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row + 1, col, curChar) == 1)
		{
			int index = (row + 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
	}

	int total = completedPoints.size();

	// calculate 2th deep if there is 1th deep
	if (total > 0)
	{
		for (int i = 0; i < _boundry.size(); i++)
		{
			int row = _boundry[i]._row;
			int col = _boundry[i]._col;

			// add if is the same color as selected color
			if (IsSameChar(row, col - 1, nextChar) == 1)
			{
				int index = row * _boardSize + col - 1;
				completedPoints.push_back(_points[index]);
			}
			// add if is the same color as selected color
			if (IsSameChar(row, col + 1, nextChar) == 1)
			{
				int index = row * _boardSize + col + 1;
				completedPoints.push_back(_points[index]);
			}
			// add if is the same color as selected color
			if (IsSameChar(row - 1, col, nextChar) == 1)
			{
				int index = (row - 1) * _boardSize + col;
				completedPoints.push_back(_points[index]);
			}
			// add if is the same color as selected color
			if (IsSameChar(row + 1, col, nextChar) == 1)
			{
				int index = (row + 1) * _boardSize + col;
				completedPoints.push_back(_points[index]);
			}
		}

		total = completedPoints.size();
	}

	// after computed, completed status is clear for later use
	for (int i = 0; i < completedPoints.size(); i++)
	{
		int row = completedPoints[i]._row;
		int col = completedPoints[i]._col;
		_points[row * _boardSize + col]._completed = false;
	}

	completedPoints.clear();
	return total;
}

// get count of characters with curChar for greedy2Deep algorithm
int CBoard::getCountByGreedy2All(char curChar, char nextChar)
{
	vector<CColorChar> completedPoints;
	for (int i = 0; i < _boundry.size(); i++)
	{
		int row = _boundry[i]._row;
		int col = _boundry[i]._col;

		// add if is the same color as selected color
		if (IsSameChar(row, col - 1, curChar) == 1)
		{
			int index = row * _boardSize + col - 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row, col + 1, curChar) == 1)
		{
			int index = row * _boardSize + col + 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row - 1, col, curChar) == 1)
		{
			int index = (row - 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row + 1, col, curChar) == 1)
		{
			int index = (row + 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
	}

	// consider newly added cell as well
	for (int i = 0; i < completedPoints.size(); i++)
	{
		int row = completedPoints[i]._row;
		int col = completedPoints[i]._col;

		// add if is the same color as selected color
		if (IsSameChar(row, col - 1, curChar) == 1)
		{
			int index = row * _boardSize + col - 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row, col + 1, curChar) == 1)
		{
			int index = row * _boardSize + col + 1;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row - 1, col, curChar) == 1)
		{
			int index = (row - 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
		// add if is the same color as selected color
		if (IsSameChar(row + 1, col, curChar) == 1)
		{
			int index = (row + 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
		}
	}
	int total = completedPoints.size();

	// calculate 2th deep if there is 1th deep
	if (total > 0)
	{
		for (int i = 0; i < _boundry.size(); i++)
		{
			int row = _boundry[i]._row;
			int col = _boundry[i]._col;

			// add if is the same color as selected color
			if (IsSameChar(row, col - 1, nextChar) == 1)
			{
				int index = row * _boardSize + col - 1;
				completedPoints.push_back(_points[index]);
			}
			// add if is the same color as selected color
			if (IsSameChar(row, col + 1, nextChar) == 1)
			{
				int index = row * _boardSize + col + 1;
				completedPoints.push_back(_points[index]);
			}
			// add if is the same color as selected color
			if (IsSameChar(row - 1, col, nextChar) == 1)
			{
				int index = (row - 1) * _boardSize + col;
				completedPoints.push_back(_points[index]);
			}
			// add if is the same color as selected color
			if (IsSameChar(row + 1, col, nextChar) == 1)
			{
				int index = (row + 1) * _boardSize + col;
				completedPoints.push_back(_points[index]);
			}
		}

		// consider newly added cell as well
		for (int i = 0; i < completedPoints.size(); i++)
		{
			int row = completedPoints[i]._row;
			int col = completedPoints[i]._col;

			// add if is the same color as selected color
			if (IsSameChar(row, col - 1, nextChar) == 1)
			{
				int index = row * _boardSize + col - 1;
				completedPoints.push_back(_points[index]);
			}
			// add if is the same color as selected color
			if (IsSameChar(row, col + 1, nextChar) == 1)
			{
				int index = row * _boardSize + col + 1;
				completedPoints.push_back(_points[index]);
			}
			// add if is the same color as selected color
			if (IsSameChar(row - 1, col, nextChar) == 1)
			{
				int index = (row - 1) * _boardSize + col;
				completedPoints.push_back(_points[index]);
			}
			// add if is the same color as selected color
			if (IsSameChar(row + 1, col, nextChar) == 1)
			{
				int index = (row + 1) * _boardSize + col;
				completedPoints.push_back(_points[index]);
			}
		}

		total = completedPoints.size();
	}

	// after computed, completed status is clear for later use
	for (int i = 0; i < completedPoints.size(); i++)
	{
		int row = completedPoints[i]._row;
		int col = completedPoints[i]._col;
		_points[row * _boardSize + col]._completed = false;
	}

	completedPoints.clear();
	return total;
}

// if exist cell with the same character as current character
bool CBoard::IsExist(char curChar)
{
	bool rtn = false;
	vector<CColorChar> completedPoints;

	// true if there is cell with same color
	for (int i = 0; i < _boundry.size(); i++)
	{
		int row = _boundry[i]._row;
		int col = _boundry[i]._col;

		// add if is the same color as selected color
		if (IsSameChar(row, col - 1, curChar) == 1)
		{
			int index = row * _boardSize + col - 1;
			completedPoints.push_back(_points[index]);
			rtn = true;
			break;
		}
		// add if is the same color as selected color
		if (IsSameChar(row, col + 1, curChar) == 1)
		{
			int index = row * _boardSize + col + 1;
			completedPoints.push_back(_points[index]);
			rtn = true;
			break;
		}
		// add if is the same color as selected color
		if (IsSameChar(row - 1, col, curChar) == 1)
		{
			int index = (row - 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
			rtn = true;
			break;
		}
		// add if is the same color as selected color
		if (IsSameChar(row + 1, col, curChar) == 1)
		{
			int index = (row + 1) * _boardSize + col;
			completedPoints.push_back(_points[index]);
			rtn = true;
			break;
		}
	}

	// after computed, completed status is clear for later use
	for (int i = 0; i < completedPoints.size(); i++)
	{
		int row = completedPoints[i]._row;
		int col = completedPoints[i]._col;
		_points[row * _boardSize + col]._completed = false;
	}

	completedPoints.clear();

	return rtn;
}

// select color of cell with the greatest count of cells adjacent to blasted region
char CBoard::getCharByAlgorithm(string algorithm)
{
	vector<char> existChars;
	if (algorithm == "greedy1adjacent")
	{
		int iMax = 0;
		for (int i = 0; i < _colorCount; i++)
		{
			int iCount = getCountByGreedy1Adjacent(ColorChars[i]);

			// exchange if new cell has larger count than max count
			if (iMax < iCount)
			{
				existChars.clear();

				// store color with the greatest count of cell
				existChars.push_back(ColorChars[i]);
				iMax = iCount;
			}
			else if (iMax == iCount) existChars.push_back(ColorChars[i]);
		}
	}
	else if (algorithm == "greedy1all")
	{
		int iMax = 0;
		for (int i = 0; i < _colorCount; i++)
		{
			int iCount = getCountByGreedy1All(ColorChars[i]);

			// exchange if new cell has larger count than max count
			if (iMax < iCount)
			{
				existChars.clear();

				// store color with the greatest count of cell
				existChars.push_back(ColorChars[i]);
				iMax = iCount;
			}
			else if (iMax == iCount) existChars.push_back(ColorChars[i]);
		}
	}
	else if (algorithm == "greedy2adjacent")
	{
		int iMax = 0;
		for (int i = 0; i < _colorCount; i++)
		{
			for (int j = 0; j < _colorCount; j++)
			{
				int iCount = getCountByGreedy2All(ColorChars[i], ColorChars[j]);

				// exchange if new cell has larger count than max count
				if (iMax < iCount)
				{
					existChars.clear();

					// store color with the greatest count of cell
					existChars.push_back(ColorChars[i]);
					iMax = iCount;
				}
				else if (iMax == iCount) existChars.push_back(ColorChars[i]);
			}
		}
	}
	else if (algorithm == "greedy2all")
	{
		int iMax = 0;
		for (int i = 0; i < _colorCount; i++)
		{
			for (int j = 0; j < _colorCount; j++)
			{
				int iCount = getCountByGreedy2All(ColorChars[i], ColorChars[j]);

				// exchange if new cell has larger count than max count
				if (iMax < iCount)
				{
					existChars.clear();

					// store color with the greatest count of cell
					existChars.push_back(ColorChars[i]);
					iMax = iCount;
				}
				else if (iMax == iCount) existChars.push_back(ColorChars[i]);
			}
		}
	}
	else if (algorithm == "random")
	{
		for (int i = 0; i < _colorCount; i++)
		{
			if (IsExist(ColorChars[i]))
			{
				// store color if is adjacent to blasted region
				existChars.push_back(ColorChars[i]);
			}
		}
	}

	// selected randomly for the cells with same count
	int index = rand() % existChars.size();
	return existChars[index];
}

// get path by various algorithm
int CBoard::runByAlgorithm(string algorithm, bool bBLZ)
{
	// create the same clone board as current board
	CBoard board(this, bBLZ);

	char curChar = 'X';
	vector<char> sequence;
	while (board._remainCount > 0)
	{
		// get current character to be selected by algorithm
		curChar = board.getCharByAlgorithm(algorithm);
		board._movesCount++;

		// blusted board by selected color
		board.ProcessBlasted(curChar);

		// store path into vector 
		sequence.push_back(curChar);
	}

	return board._movesCount;

	sequence.clear();
}

// run until 'Q' is pressed
void CBoard::run(int boardSize)
{
	// Initialize random number seed generator	
	string outputFileName = "inside" + to_string((_Longlong)boardSize) + ".csv";

	// change the count as you want like 1000000
	int count = 100000;
	if(boardSize > 96) count = 1000;

	ofstream oFile(outputFileName);//, ios::app);
	if(!oFile.is_open())
	{
		cout << outputFileName << " can't be opened." << endl;
		return;
	}

	cout << "inside..." << endl;
	srand(time(0));
	if(!boardInit(boardSize)) 
	{
		oFile.close();
		return;
	}
	oFile  << "#RAN,#G1J,#G1L,#G2J,#G2L" << endl;
	int i = 0;
	for(i = 0; i < count; i++)
	{
		oFile << runByAlgorithm("random", true) << ",";
		oFile << runByAlgorithm("greedy1adjacent", true) << ",";
		oFile << runByAlgorithm("greedy1all", true) << ",";
		oFile << runByAlgorithm("greedy2adjacent", true) << ",";
		oFile << runByAlgorithm("greedy2all", true) << endl;
		cout << i << " of " <<  count << ".\r";
	}
	cout << i << " of " <<  count << ".\r" << endl << "Completed siccessfully." << endl << endl;
	oFile.close();

	outputFileName = "outside" + to_string((_Longlong)boardSize) + ".csv";
	oFile.open(outputFileName);//, ios::app);
	if(!oFile.is_open())
	{
		cout << outputFileName << " can't be opened." << endl;
		return;
	}

	cout << "outside..." << endl;
	srand(time(0));
	oFile  << "#RAN,#G1J,#G1L,#G2J,#G2L" << endl;
	i = 0;
	for(i = 0; i < count; i++)
	{
		oFile << runByAlgorithm("random", false) << ",";
		oFile << runByAlgorithm("greedy1adjacent", false) << ",";
		oFile << runByAlgorithm("greedy1all", false) << ",";
		oFile << runByAlgorithm("greedy2adjacent", false) << ",";
		oFile << runByAlgorithm("greedy2all", false) << endl;
		cout << i << " of " <<  count << ".\r";
	}
	cout << i << " of " <<  count << ".\r" << endl << "Completed siccessfully." << endl << endl;
	oFile.close();
}