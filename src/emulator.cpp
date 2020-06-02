#include <iostream>

#include "chip8/chip8.h"

int main()
{
	chip8 emu;

	std::cout << emu.display_memory_size << '\n';
	
	return 0;
}
