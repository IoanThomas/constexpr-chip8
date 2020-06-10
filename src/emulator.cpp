#include "emulator.h"

#include <algorithm>
#include <fstream>
#include <ios>
#include <iterator>
#include <vector>

emulator::emulator(const std::string& rom_file_path)
{
	load_rom(rom_file_path);
}

void emulator::run()
{
}

void emulator::load_rom(const std::string& rom_file_path)
{
	std::ifstream file(rom_file_path, std::ios::binary);

	uint16_t count = 0;
	std::for_each(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), [this, &count](const char c) {
		m_chip8.memory[chip8::program_memory_start + count] = static_cast<uint8_t>(c);
		++count;
	});
}
