#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "chip8/chip8.h"

template<bool test> bool static_test()
{
	static_assert(test);
	return test;
}

#define TEST(x) static_test<x>()

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

	REQUIRE(TEST(emu.is_pixel_set(0, 0) == false));
	REQUIRE(TEST(emu.is_pixel_set(63, 0) == false));
	REQUIRE(TEST(emu.is_pixel_set(0, 31) == false));
	REQUIRE(TEST(emu.is_pixel_set(63, 31) == false));
}

TEST_CASE("00EE", "[opcode]")
{
	constexpr auto emu = run(0x22, 0x02, 0x00, 0xEE);

	REQUIRE(TEST(emu.program_counter == 514));
}
