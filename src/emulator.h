#pragma once

#include <string>

#include "chip8/chip8.h"

class emulator
{
public:
	emulator(const std::string& rom_file_path);

	void run();

private:
	void load_rom(const std::string& rom_file_path);

private:
	chip8 m_chip8;
};
