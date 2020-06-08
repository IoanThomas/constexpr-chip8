#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

#include "chip8/chip8.h"

template<bool test>
bool static_test()
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

TEST_CASE("00EE returns from a subroutine", "[opcode]")
{
	constexpr auto emu = run(0x22, 0x04, 0x00, 0xEE, 0x00, 0xEE);

	REQUIRE(TEST(emu.program_counter == 514));
	REQUIRE(TEST(emu.stack_pointer == 0));
}

TEST_CASE("1NNN jumps to address NNN", "[opcode]")
{
	constexpr auto emu = run(0x12, 0x04, 0x00, 0xEE, 0x00, 0xEE);

	REQUIRE(TEST(emu.program_counter == 516));
}

TEST_CASE("2NNN calls a subroutine at NNN", "[opcode]")
{
	constexpr auto emu = run(0x22, 0x04, 0x00, 0xEE);

	REQUIRE(TEST(emu.program_counter == 516));
	REQUIRE(TEST(emu.stack_pointer == 1));
}

TEST_CASE("3XNN skips the next instruction if Vx equals NN", "[opcode]")
{
	constexpr auto emu = run(0x60, 0x12, 0x30, 0x12, 0x00, 0xEE, 0x00, 0xEE);

	REQUIRE(TEST(emu.program_counter == 518));
}

TEST_CASE("4XNN skips the next instruction if Vx does not equal NN", "[opcode]")
{
	constexpr auto emu = run(0x60, 0x12, 0x40, 0x12, 0x00, 0xEE, 0x00, 0xEE);

	REQUIRE(TEST(emu.program_counter == 516));
}
