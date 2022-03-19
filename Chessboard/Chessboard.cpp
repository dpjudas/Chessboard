
#include "SPIDevice.h"
#include "MFRC522.h"
#include <gpiod.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <cstdint>
#include <memory>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <time.h>

void delay(int ns)
{
	timespec ts = {};
	ts.tv_sec = 0;
	ts.tv_nsec = ns;
	int result = nanosleep(&ts, nullptr);
	if (result < 0)
		throw std::runtime_error("nanosleep failed");
}

int main(int argc, char** argv)
{
	try
	{
		auto spi = std::make_unique<SPIDevice>();
		auto rfid = std::make_unique<MFRC522>(spi.get());

		const char* consumer = "chess";

		gpiod_chip* chip = gpiod_chip_open_by_name("gpiochip0");
		if (!chip)
			throw std::runtime_error("Could not open gpio");

		gpiod_line* resetpin = gpiod_chip_get_line(chip, 14);
		gpiod_line_request_output(resetpin, consumer, 0);

		gpiod_line_set_value(resetpin, 0);
		delay(50);
		gpiod_line_set_value(resetpin, 1);
		delay(50);

		rfid->PCD_Init();

		while (true)
		{
			if (rfid->PICC_IsNewCardPresent() && rfid->PICC_ReadCardSerial())
			{
				MFRC522::PICC_Type piccType = rfid->PICC_GetType(rfid->uid.sak);
				std::string strID;
				for (int i = 0; i < 10; i++)
				{
					if (i > 0)
						strID += ":";

					int v = rfid->uid.uidByte[i];
					for (int v0 : { v >> 4, v & 15 })
					{
						if (v0 < 10)
							strID += '0' + v0;
						else
							strID += 'a' + (v0 - 10);
					}
				}
				std::cout << rfid->PICC_GetTypeName(piccType).c_str() << ": " << strID.c_str() << std::endl;
			}
		}

/*
		gpiod_line* gpio14 = gpiod_chip_get_line(chip, 14);
		gpiod_line* gpio15 = gpiod_chip_get_line(chip, 15);
		gpiod_line* gpio18 = gpiod_chip_get_line(chip, 18);

		gpiod_line_request_input(gpio14, consumer);
		int value14 = gpiod_line_get_value(gpio14);
		std::cout << "gpio14 is " << value14 << std::endl;

		gpiod_line_request_input(gpio15, consumer);
		int value15 = gpiod_line_get_value(gpio15);
		std::cout << "gpio15 is " << value15 << std::endl;

		// gpiod_line_request_output(gpio18, consumer, 0);
		// gpiod_line_set_value(gpio18, 1);

		gpiod_line_release(gpio18);
		gpiod_line_release(gpio14);
		gpiod_line_release(gpio15);

*/
		gpiod_line_release(resetpin);
		gpiod_chip_close(chip);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 255;
	}

	return 0;
}
