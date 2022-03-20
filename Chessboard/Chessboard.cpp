
#include "SPIDevice.h"
#include "MFRC522.h"
#include "GPIODevice.h"
#include "Timer.h"
#include <iostream>

int main(int argc, char** argv)
{
	try
	{
		auto gpio = std::make_unique<GPIODevice>("chess");
		auto spi = std::make_unique<SPIDevice>();
		auto rfid = std::make_unique<MFRC522>(spi.get());

		// Reset everything on the bus
		int resetpin = 14;
		gpio->request_output(resetpin, false);
		delay(50);
		gpio->set_value(resetpin, true);

		rfid->PCD_Init();

		while (true)
		{
			std::string card = rfid->ReadCurrentCard();
			if (!card.empty())
			{
				std::cout << card.c_str() << std::endl;
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 255;
	}

	return 0;
}
