#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "chip8/chip8.h"

template<typename... T>
constexpr auto run(T... program)
{
	std::array<uint8_t, sizeof...(T)> data{ static_cast<uint8_t>(program)... };

	chip8 emu{ data };
	emu.run();

	return emu;
}

TEST_CASE("00E0 resets all pixels", "[opcode]")
{
	constexpr auto emu = run(0xD0, 0x15, 0x00, 0xE0);
	bool pixels_set = false;

	for (auto y = 0; y < emu.display_height; ++y)
		for (auto x = 0; x < emu.display_width; ++x)
			if (emu.is_pixel_set(x, y))
				pixels_set = true;

	REQUIRE_FALSE(pixels_set);
}
