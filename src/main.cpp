#include <cstdlib>
#include <iostream>

#include "emulator.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "Please specifiy a path to a CHIP-8 ROM\n";
		return EXIT_FAILURE;
	}

	emulator emu(argv[1]);
	emu.run();

	return EXIT_SUCCESS;
}
