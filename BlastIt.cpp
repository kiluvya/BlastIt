// BlastIt.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <string>
#include <sstream>
#include "Board.h"

using namespace std;

// main
int main(int argc, char* argv[])
{
	while(true)
	{
		cout << "Kiluvya's Shock and Awe" << endl;
		cout << "Press 'Q' key to exit." << endl;
		cout << "Enter board size(16, 32, 48, 64, 80, 96, 128, 256, 384, 512, 640, 768, 896): ";

		string c;
		cin >> c;
		if(c == "q" | c == "Q") return -1;

		int boardSize;

		// convert string to int
		stringstream(c) >> boardSize;

		// return when value is between 12 and 2100
		if(boardSize == 16 || boardSize == 32 || boardSize == 48 || boardSize == 64 || boardSize == 80 || 
			boardSize == 96 || boardSize == 128 || boardSize == 256 || boardSize == 384 || boardSize == 512 || 
			boardSize == 640 || boardSize == 768 || boardSize == 896) 
		{
			CBoard board;
			board.run(boardSize);
		}
		else cout << "Invalid board size." << endl << endl;
	}

	return 0;
}

