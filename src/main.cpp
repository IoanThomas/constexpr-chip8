#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "emulator.h"

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Please specifiy a path to a CHIP-8 ROM\n";
		return EXIT_FAILURE;
	}

	try
	{
		emulator emu(argv[1]);
		emu.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}

	return EXIT_SUCCESS;
}
