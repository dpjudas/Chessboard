
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <map>
#include <gpiod.h>

class GPIODevice
{
public:
	GPIODevice(const std::string& consumer);
	~GPIODevice();

	void request_input(int line);
	void request_output(int line, bool initial_value = false);

	void set_value(int line, bool value);
	bool get_value(int line);

private:
	gpiod_line* get_line(int line);

	GPIODevice(const GPIODevice&) = delete;
	GPIODevice& operator=(const GPIODevice&) = delete;

	std::string consumer;
	gpiod_chip* chip = nullptr;
	std::map<int, gpiod_line*> lines;
};
