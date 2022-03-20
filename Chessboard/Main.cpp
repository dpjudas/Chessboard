
#include "Chessboard.h"
#include <iostream>
#include <stdexcept>

int main(int argc, char** argv)
{
	try
	{
		Chessboard chessboard;
		chessboard.run();
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 255;
	}
}
