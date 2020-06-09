#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

class chip8
{
public:
	static constexpr std::array<uint8_t, 80> font = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	// Emulator specifications
	static constexpr uint16_t memory_size = 4096;
	static constexpr uint8_t display_width = 64;
	static constexpr uint8_t display_height = 32;
	static constexpr uint8_t max_stacks = 12;

	// Sizes used for later calculations
	static constexpr uint16_t display_memory_size = (display_width * display_height) / 8; // Each pixel is only a single bit
	static constexpr uint16_t stack_size = max_stacks * 4; // 4 byte stack pointers
	static constexpr uint16_t reserved_memory_size = 96; // Reserved for the call stack and address register
	
	static constexpr uint16_t display_memory_start = memory_size - display_memory_size;
	static constexpr uint16_t call_stack_start = display_memory_start - reserved_memory_size;

	static constexpr uint16_t program_memory_start = 512; // The first 512 bytes are for internal use only
	static constexpr uint16_t program_memory_end = call_stack_start;

	static_assert(program_memory_end - program_memory_start > 0, "No memory for programs");

	struct
	{
		std::array<uint8_t, 16> data{};
		uint16_t address = 0;
	} registers;

	std::array<uint8_t, memory_size> memory{};
	uint16_t program_counter = program_memory_start;
	std::array<uint16_t, 12> call_stack{};
	uint8_t stack_pointer = 0;

	uint8_t delay_timer = 60;
	uint8_t sound_timer = 60;

	bool draw_flag = false;

	template<std::size_t size>
	constexpr chip8(const std::array<uint8_t, size>& program) noexcept
	{
		assert(size <= program_memory_end - program_memory_start);

		// Copy the font into reserved memory
		for (auto i = 0; i < font.size(); ++i)
			memory[i] = font[i];

		// Copy the program into memory
		for (auto i = 0; i < size; ++i)
			memory[static_cast<typename decltype(memory)::size_type>(program_memory_start) + i] = program[i];
	}

	constexpr void run() noexcept
	{
		while (next_instruction()) {}
	}

	constexpr bool next_instruction() noexcept
	{
		// Memory is stored as single bytes, but instructions are two bytes each, so we combine OR them together
		const uint16_t instruction = (memory[program_counter] << 8) | memory[program_counter + 1];

		return instruction > 0 ? evaluate_instruction(instruction) : false;
	}

	constexpr bool evaluate_instruction(const uint16_t instruction) noexcept
	{
		bool continue_running = true;
		const uint16_t opcode_major = instruction & 0xF000;

		switch (opcode_major)
		{
		case 0x0000:
			switch (instruction & 0x00FF)
			{
			case 0x00E0: // 00E0 - Clear screen
				clear_screen();
				draw_flag = true;

				program_counter += 2;
				break;
			case 0x00EE: // 00EE - Return from subroutine
				if (stack_pointer == 0)
				{
					// Quit program if we are in the starting subroutine
					continue_running = false;
					break;
				}

				program_counter = call_stack[--stack_pointer];
				break;
			}
			break;
		case 0x1000: // 1NNN - Jump to address NNN
			{
				const uint16_t jump_address = instruction & 0x0FFF;

				program_counter = jump_address;
			}
			break;
		case 0x2000: // 2NNN - Call subroutine at NNN
			{
				const uint16_t subroutine_address = instruction & 0x0FFF;

				auto x = program_counter + 2;
				call_stack[stack_pointer++] = program_counter + 2;
				program_counter = subroutine_address;
			}
			break;
		case 0x3000: // 3XNN - Skip next instruction if Vx = NN
			{
				const uint8_t registr = (instruction & 0x0F00) >> 8;
				const uint8_t value = instruction & 0x00FF;

				if (registers.data[registr] == value)
					program_counter += 2;

				program_counter += 2;
			}
			break;
		case 0x4000: // 4XNN - Skip next instruction if Vx =/= NN
			{
				const uint8_t registr = (instruction & 0x0F00) >> 8;
				const uint8_t value = instruction & 0x00FF;

				if (registers.data[registr] != value)
					program_counter += 2;

				program_counter += 2;
			}
			break;
		case 0x5000: // 5XY0 - Skip next instruction if Vx = Vy
			{
				const uint8_t register_x = (instruction & 0x0F00) >> 8;
				const uint8_t register_y = (instruction & 0x00F0) >> 4;

				if (registers.data[register_x] == registers.data[register_y])
					program_counter += 2;

				program_counter += 2;
			}
			break;
		case 0x6000: // 6XNN - Set Vx to NN
			{
				const uint8_t registr = (instruction & 0x0F00) >> 8;
				const uint8_t value = instruction & 0x00FF;

				registers.data[registr] = value;

				program_counter += 2;
			}
			break;
		case 0x7000: // 7XNN - Add NN to Vx
			{
				const uint8_t registr = (instruction & 0x0F00) >> 8;
				const uint8_t value = instruction & 0x00FF;

				registers.data[registr] += value;

				program_counter += 2;
			}
			break;
		case 0x8000:
			{
				const uint8_t register_x = (instruction & 0x0F00) >> 8;
				const uint8_t register_y = (instruction & 0x00F0) >> 4;

				switch (instruction & 0x000F)
				{
				case 0x0000: // 8XY0 - Set Vx to Vy
					registers.data[register_x] = registers.data[register_y];
					program_counter += 2;
					break;
				case 0x0001: // 8XY1 - Set Vx to Vx OR Vy
					registers.data[register_x] |= registers.data[register_y];
					program_counter += 2;
					break;
				case 0x0002: // 8XY2 - Set Vx to Vx AND Vy
					registers.data[register_x] &= registers.data[register_y];
					program_counter += 2;
					break;
				case 0x0003: // 8XY3 - Set Vx to Vx XOR Vy
					registers.data[register_x] ^= registers.data[register_y];
					program_counter += 2;
					break;
				case 0x0004: // 8XY4 - Add Vy to Vx, set Vf to 1 if there is a carry, and 0 otherwise
					registers.data[0xF] = (registers.data[register_y] > (0xFF - registers.data[register_x])) ? 1 : 0;
					registers.data[register_x] += registers.data[register_y];
					program_counter += 2;
					break;
				case 0x0005: // 8XY5 - Subtract Vy from Vx, set Vf to 0 if there is a borrow, and 1 otherwise
					registers.data[0xF] = (registers.data[register_y] > registers.data[register_x]) ? 0 : 1;
					registers.data[register_x] -= registers.data[register_y];
					program_counter += 2;
					break;
				case 0x0006: // 8XY6 - Store the least significant bit of Vx in Yf, and right shift Vx by 1
					registers.data[0xF] = registers.data[register_x] & 1;
					registers.data[register_x] >>= 1;
					program_counter += 2;
					break;
				case 0x0007: // 8XY7 - Set Vx to Vy - Vx, set Vf to 0 if there is a borrow, and 1 otherwise
					registers.data[0xF] = (registers.data[register_x] > registers.data[register_y]) ? 0 : 1;
					registers.data[register_x] = registers.data[register_y] - registers.data[register_x];
					program_counter += 2;
					break;
				case 0x000E: // 8XYE - Store the most significant bit of Vx in Vf, and left shift Vx by 1
					registers.data[0xF] = registers.data[register_x] >> 7;
					registers.data[register_x] <<= 1;
					program_counter += 2;
					break;
				}
				break;
			}
		case 0x9000: // 9XY0 - Skip next instruction if Vx =/= Vy
			{
				const uint8_t register_x = (instruction & 0x0F00) >> 8;
				const uint8_t register_y = (instruction & 0x00F0) >> 4;

				if (registers.data[register_x] != registers.data[register_y])
					program_counter += 2;

				program_counter += 2;
			}
			break;
		case 0xA000: // ANNN - Set the address register to NNN
			{
				const uint8_t value = instruction & 0x0FFF;

				registers.address = value;

				program_counter += 2;
			}
			break;
		case 0xB000: // BNNN - Jump to address NNN + V0
			{
				const uint16_t jump_address = (instruction & 0x0FFF) + registers.data[0x0];

				program_counter = jump_address;
			}
			break;
		case 0xC000: // CXNN - Set Vx to a random number AND NN
			{
				const uint8_t registr = (instruction & 0x0F00) >> 8;
				const uint8_t value = instruction & 0x00FF;

				// TODO: Generate random numbers at both compile-time and runtime
				//m_registers[registr] = random_number() & value;

				program_counter += 2;
			}
			break;
		case 0xD000: // DXYN - Draw sprite located at address register at (Vx,Vy), with a height of N
			{
				const uint8_t register_x = (instruction & 0x0F00) >> 8;
				const uint8_t register_y = (instruction & 0x00F0) >> 4;
				const uint8_t height = instruction & 0x000F;

				draw_sprite(registers.data[register_x], registers.data[register_y], height);

				program_counter += 2;
			}
			break;
		case 0xE000:
			{
				const uint8_t registr = (instruction & 0x0F00) >> 8;

				switch (instruction & 0x00FF)
				{
				case 0x009E: // EX9E - Skip next instruction if key stored in Vx is pressed
					// TODO: Keyboard input

					program_counter += 2;
					break;
				case 0x00A1: // EXA1 - Skip next instruction if key stored in Vx is not pressed
					// TODO: Keyboard input

					program_counter += 2;
					break;
				}
			}
			break;
		case 0xF000:
			{
				const uint8_t registr = (instruction & 0x0F00) >> 8;

				switch (instruction & 0x00FF)
				{
				case 0x0007: // FX07 - Set Vx to the value of the delay timer
					registers.data[registr] = delay_timer;
					program_counter += 2;
					break;
				case 0x000A: // FX0A - Wait for a key press and store it in Vx
					// TODO: Keyboard input

					program_counter += 2;
					break;
				case 0x0015: // FX15 - Set the delay timer to Vx
					delay_timer = registers.data[registr];
					program_counter += 2;
					break;
				case 0x0018: // FX18 - Set the sound timer to Vx
					sound_timer = registers.data[registr];
					program_counter += 2;
					break;
				case 0x001E: // FX1E - Add Vx to address register, set Vf to 1 if there is an overflow, and 0 otherwise
					registers.data[0xF] = (registers.data[registr] + registers.address > 0xFFF) ? 1 : 0;
					registers.address += registers.data[registr];
					program_counter += 2;
					break;
				case 0x0029: // FX29 - Set address register to location of sprite for character in Vx
					registers.address = registers.data[registr] * 5;
					program_counter += 2;
					break;
				case 0x0033: // FX33 - Store binary-coded decimal representation of Vx in address register and next two locations
					memory[registers.address] = registers.data[registr] / 100;
					memory[registers.address + 1] = (registers.data[registr] / 10) % 10;
					memory[registers.address + 2] = registers.data[registr] % 10;
					program_counter += 2;
					break;
				case 0x0055: // FX55 - Store V0 to Vx in memory starting at address register
					for (auto i = 0; i < registr; ++i)
						memory[registers.address + i] = registers.data[i];

					program_counter += 2;
					break;
				case 0x0065: // FX65 - Fill V0 to Vx with values starting at address register
					for (auto i = 0; i < registr; ++i)
						registers.data[i] = memory[registers.address + i];

					program_counter += 2;
					break;
				}
			}
			break;
		}

		if (delay_timer > 0)
			--delay_timer;

		if (sound_timer > 0)
			--sound_timer;

		return continue_running;
	}

	constexpr void draw_sprite(const uint8_t x_pos, const uint8_t y_pos, const uint8_t height) noexcept
	{
		bool pixels_inverted = false;

		for (auto y = 0; y < height; ++y)
		{
			const auto row = memory[registers.address + y];

			for (auto x = 0; x < 8; ++x)
			{
				const bool bit = (row >> (7 - (x % 8))) & 1;

				if (bit)
				{
					if (is_pixel_set(x_pos + x, y_pos + y))
						pixels_inverted = true;

					invert_pixel(x_pos + x, y_pos + y);
				}
			}
		}

		registers.data[0xF] = pixels_inverted ? 1 : 0;
		draw_flag = true;
	}

	constexpr bool is_pixel_set(const uint8_t x_pos, const uint8_t y_pos) const noexcept
	{
		const auto byte = memory[display_memory_start + (y_pos * display_width / 8) + (x_pos / 8)];
		return (byte >> (7 - (x_pos % 8))) & 1;
	}

	constexpr void invert_pixel(const uint8_t x_pos, const uint8_t y_pos) noexcept
	{
		auto& byte = memory[display_memory_start + (y_pos * display_width / 8) + (x_pos / 8)];
		byte ^= (1 << (7 - (x_pos % 8)));
	}

	constexpr void clear_screen() noexcept
	{
		for (auto i = 0; i < display_memory_size; ++i)
			memory[display_memory_start + i] = 0;
	}
};
