#include "chip8/chip8.h"

int main()
{
	constexpr std::array<uint8_t, 2> program = { 0x63, 0x55 };
	constexpr chip8 emu(program);

	return 0;
}
