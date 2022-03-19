
#include "SPIDevice.h"
#include <stdexcept>
#include <string>
#include <memory>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <time.h>

SPIDevice::SPIDevice(uint8_t mode, uint8_t bits_per_word, uint32_t speed, uint16_t delay)
{
	std::string devicename = "/dev/spidev0.0";
	handle = open(devicename.c_str(), O_RDWR);
	if (handle < 0)
		throw std::runtime_error("Could not open " + devicename);

	int result = ioctl(handle, SPI_IOC_WR_MODE, &mode);
	if (result < 0)
	{
		close(handle);
		throw std::runtime_error("SPI_IOC_WR_MODE failed");
	}

	result = ioctl(handle, SPI_IOC_RD_MODE, &mode);
	if (result < 0)
	{
		close(handle);
		throw std::runtime_error("SPI_IOC_RD_MODE failed");
	}

	result = ioctl(handle, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word);
	if (result < 0)
	{
		close(handle);
		throw std::runtime_error("SPI_IOC_WR_BITS_PER_WORD failed");
	}

	result = ioctl(handle, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word);
	if (result < 0)
	{
		close(handle);
		throw std::runtime_error("SPI_IOC_RD_BITS_PER_WORD failed");
	}

	result = ioctl(handle, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (result < 0)
	{
		close(handle);
		throw std::runtime_error("SPI_IOC_WR_MAX_SPEED_HZ failed");
	}

	result = ioctl(handle, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (result < 0)
	{
		close(handle);
		throw std::runtime_error("SPI_IOC_RD_MAX_SPEED_HZ failed");
	}
}

SPIDevice::~SPIDevice()
{
	close(handle);
}

int SPIDevice::transfer(const void* output, void* input, size_t size)
{
	spi_ioc_transfer desc = {};
	desc.rx_buf = (__u64)input;
	desc.tx_buf = (__u64)output;
	desc.len = size;
	int result = ioctl(handle, SPI_IOC_MESSAGE(1), &desc);
	if (result < 0)
		throw std::runtime_error("SPI transfer failed");
	return result;
}

int SPIDevice::read(void* data, size_t size)
{
	spi_ioc_transfer desc = {};
	desc.rx_buf = (__u64)data;
	desc.len = size;
	int result = ioctl(handle, SPI_IOC_MESSAGE(1), &desc);
	if (result < 0)
		throw std::runtime_error("SPI read failed");
	return result;
}

int SPIDevice::write(const void* data, size_t size)
{
	spi_ioc_transfer desc = {};
	desc.tx_buf = (__u64)data;
	desc.len = size;
	int result = ioctl(handle, SPI_IOC_MESSAGE(1), &desc);
	if (result < 0)
		throw std::runtime_error("SPI write failed");
	return result;
}
