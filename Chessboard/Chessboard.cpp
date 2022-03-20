
#include "Chessboard.h"
#include "Timer.h"
#include <iostream>
#include <curses.h>

static const int num_connected_rfids = 2; // increase up to 16

Chessboard::Chessboard()
{
	gpio = std::make_unique<GPIODevice>("chess");
	spi = std::make_unique<SPIDevice>();
	rfid = std::make_unique<MFRC522>(spi.get());

	// Init gpio
	gpio->request_output(resetpin, false);
	for (int i = 0; i < 4; i++)
		gpio->request_output(muxselectpins[i], false);

	// Reset the bus
	gpio->set_value(resetpin, false);
	delay(50);
	gpio->set_value(resetpin, true);

	// Reset the rfid chips
	for (int i = 0; i < num_connected_rfids; i++)
	{
		delay(200); // test delay

		// Select the chip
		for (int j = 0; j < 4; j++)
		{
			gpio->set_value(muxselectpins[j], (i & (1 << j)) != 0);
		}

		// We must wait 50 ns before talking to the chip after selection
		delay(50);

		// Check if we can talk to the chip at all
		if (!rfid->PCD_PerformSelfTest())
		{
			throw std::runtime_error("Self test failed for rfid chip #" + std::to_string(i));
		}

		// Send init code
		rfid->PCD_Init();
	}

	init_curses();
}

void Chessboard::run()
{
	std::map<std::string, int> seenCards;
	std::vector<std::string> seenCardIndexNames;
	seenCardIndexNames.push_back("(empty)");
	int nextId = 1;

	while (true)
	{
		for (int i = 0; i < num_connected_rfids; i++)
		{
			delay(200); // test delay

			// Select the chip
			for (int j = 0; j < 4; j++)
			{
				gpio->set_value(muxselectpins[j], (i & (1 << j)) != 0);
			}

			// We must wait 50 ns before talking to the chip after selection
			delay(50);

			// Query the chip
			tiles[i] = rfid->ReadCurrentCard();
		}

		// Assign a number to the card if it hasn't been seen before
		for (int i = 0; i < 64; i++)
		{
			if (!tiles[i].empty() && seenCards[tiles[i]] == 0)
			{
				seenCardIndexNames.push_back(tiles[i]);
				seenCards[tiles[i]] = nextId++;
			}
		}

		// Print the list of seen cards
		mvprintw(0, 40, "RFID Cards:");
		for (int i = 1; i < nextId; i++)
		{
			mvprintw(i, 40, "#%d %s", i, seenCardIndexNames[i].c_str());
		}

		// Print the board
		for (int y = 0; y < 8; y++)
		{
			for (int x = 0; x < 8; x++)
			{
				int idx = x + y * 8;

				mvprintw(y * 2, x * 3, "+--+");
				if (!tiles[idx].empty())
				{
					mvprintw(y * 2 + 1, x * 3, x == 7 ? "|%02d" : "|%02d", seenCards[tiles[idx]]);
				}
				else
				{
					mvprintw(y * 2 + 1, x * 3, x == 7 ? "|  |" : "|  ");
				}

				if (y == 7)
					mvprintw(y * 2 + 2, x * 3, x == 7 ? "+--+" : "+--");
			}
		}

		refresh();
	}
}

void Chessboard::init_curses()
{
	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	noecho();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	clear();
}
