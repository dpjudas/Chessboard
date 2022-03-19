
#pragma once

#include <cstdint>
#include <memory>

class SPIDevice
{
public:
	SPIDevice(uint8_t mode = 0, uint8_t bits_per_word = 8, uint32_t speed = 1000000, uint16_t delay = 0);
	~SPIDevice();

	int transfer(const void* output, void* input, size_t size);
	int read(void* data, size_t size);
	int write(const void* data, size_t size);

private:
	SPIDevice(const SPIDevice&) = delete;
	SPIDevice& operator=(const SPIDevice&) = delete;

	int handle = -1;
};
