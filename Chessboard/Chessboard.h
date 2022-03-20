
#pragma once

#include "GPIODevice.h"
#include "SPIDevice.h"
#include "MFRC522.h"
#include <vector>
#include <string>

class Chessboard
{
public:
	Chessboard();
	void run();

private:
	void init_curses();

	std::unique_ptr<GPIODevice> gpio;
	std::unique_ptr<SPIDevice> spi;
	std::unique_ptr<MFRC522> rfid;
	std::string tiles[64];

	int resetpin = 14;
	int muxselectpins[4] = { 24, 25, 27, 22 };
};
