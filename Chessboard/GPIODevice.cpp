
#include "GPIODevice.h"
#include <stdexcept>

GPIODevice::GPIODevice(const std::string& consumer) : consumer(consumer)
{
	chip = gpiod_chip_open_by_name("gpiochip0");
	if (!chip)
		throw std::runtime_error("Could not open gpiochip0");
}

GPIODevice::~GPIODevice()
{
	for (auto& it : lines)
		gpiod_line_release(it.second);
	lines.clear();
	gpiod_chip_close(chip);
	chip = nullptr;
}

void GPIODevice::request_input(int line)
{
	gpiod_line_request_input(get_line(line), consumer.c_str());
}

void GPIODevice::request_output(int line, bool initial_value)
{
	gpiod_line_request_output(get_line(line), consumer.c_str(), initial_value ? 1 : 0);
}

void GPIODevice::set_value(int line, bool value)
{
	gpiod_line_set_value(get_line(line), value ? 1 : 0);
}

bool GPIODevice::get_value(int line)
{
	return gpiod_line_get_value(get_line(line)) != 0;
}

gpiod_line* GPIODevice::get_line(int line)
{
	auto it = lines.find(line);
	if (it != lines.end())
		return it->second;

	gpiod_line* handle = gpiod_chip_get_line(chip, line);
	if (!handle)
		throw std::runtime_error("Could not get gpio line " + std::to_string(line));

	try
	{
		lines[line] = handle;
	}
	catch (...)
	{
		gpiod_line_release(handle);
		throw;
	}

	return handle;
}
