#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

template<uint16_t memory_size = 4096>
class chip8
{
public:
	// Emulator specifications
	static constexpr uint8_t display_width = 64;
	static constexpr uint8_t display_height = 32;
	static constexpr uint8_t max_stacks = 12;

	// Sizes used for later calculations
	static constexpr uint16_t display_memory_size = (display_width * display_height) / 8; // Each pixel is only a single bit
	static constexpr uint16_t stack_size = max_stacks * 4; // 4 byte stack pointers
	static constexpr uint16_t reserved_memory_size = 96; // Reserved for the call stack and address register
	
	static constexpr uint16_t display_memory_start = memory_size - display_memory_size;
	static constexpr uint16_t call_stack_start = display_memory_start - reserved_memory_size;
	static constexpr uint16_t address_register = call_stack_start + stack_size;

	static constexpr uint16_t program_memory_start = 512; // The first 512 bytes are for internal use only
	static constexpr uint16_t program_memory_end = call_stack_start;

	static_assert(program_memory_end - program_memory_start > 0, "No memory for programs");

	template<std::size_t size>
	constexpr chip8(const std::array<uint8_t, size>& program) noexcept
	{
		assert(size <= program_memory_end - program_memory_start);

		// Copy the program into memory
		for (auto i = 0; i < size; ++i)
			m_memory[static_cast<typename decltype(m_memory)::size_type>(program_memory_start) + i] = program[i];
	}

	constexpr void run() noexcept
	{
		while (next_instruction()) {}
	}

	constexpr bool next_instruction() noexcept
	{
		// Memory is stored as single bytes, but instructions are two bytes each, so we combine OR them together
		const uint16_t instruction = (m_memory[m_program_counter] << 8) | m_memory[m_program_counter + 1];
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
			case 0xE0: // 00E0 - Clear screen
				clear_screen();
				m_draw_flag = true;

				break;
			case 0xEE: // 00EE - Return from subroutine
				if (m_stack_pointer <= call_stack_start)
					continue_running = false; // Quit program if we are in the starting subroutine

				m_program_counter = m_memory[--m_stack_pointer];
				break;
			}
			break;
		case 0x1000: // 1NNN - Jump to address
			{
				const uint16_t jump_address = instruction & 0x0FFF;

				m_program_counter = jump_address;
			}
			break;
		case 0x2000: // 2NNN - Call subroutine
			{
				const uint16_t subroutine_address = instruction & 0x0FFF;

				m_memory[m_stack_pointer++] = m_program_counter + 2;
				m_program_counter = subroutine_address;
			}
			break;
		case 0x3000: // 3XNN - Skip next instruction if Vx = NN
			{
				const uint8_t registr = (instruction & 0x0F00) >> 8;
				const uint8_t value = instruction & 0x00FF;

				if (m_registers[registr] == value)
					m_program_counter += 2;

				m_program_counter += 2;
			}
			break;
		case 0x4000: // 4XNN - Skip next instruction if Vx =/= NN
			{
				const uint8_t registr = (instruction & 0x0F00) >> 8;
				const uint8_t value = instruction & 0x00FF;

				if (m_registers[registr] != value)
					m_program_counter += 2;

				m_program_counter += 2;
			}
			break;
		case 0x5000: // 5XY0 - Skip next instruction if Vx = Vy
			{
				const uint8_t register_x = (instruction & 0x0F00) >> 8;
				const uint8_t register_y = (instruction & 0x00F0) >> 4;

				if (m_registers[register_x] == m_registers[register_y])
					m_program_counter += 2;

				m_program_counter += 2;
			}
			break;
		case 0x6000: // 6XNN - Set Vx to NN
			{
				const uint8_t registr = (instruction & 0x0F00) >> 8;
				const uint8_t value = instruction & 0x00FF;

				m_registers[registr] = value;

				m_program_counter += 2;
			}
			break;
		case 0x7000: // 7XNN - Add NN to Vx
			{
				const uint8_t registr = (instruction & 0x0F00) >> 8;
				const uint8_t value = instruction & 0x00FF;

				m_registers[registr] += value;

				m_program_counter += 2;
			}
			break;
		case 0x8000:
			{
				const uint8_t register_x = (instruction & 0x0F00) >> 8;
				const uint8_t register_y = (instruction & 0x00F0) >> 4;

				switch (instruction & 0x000F)
				{
				case 0x0000: // 8XY0 - Set Vx to Vy
					m_registers[register_x] = m_registers[register_y];
					m_program_counter += 2;
					break;
				case 0x0001: // 8XY1 - Set Vx to Vx OR Vy
					m_registers[register_x] |= m_registers[register_y];
					m_program_counter += 2;
					break;
				case 0x0002: // 8XY2 - Set Vx to Vx AND Vy
					m_registers[register_x] &= m_registers[register_y];
					m_program_counter += 2;
					break;
				case 0x0003: // 8XY3 - Set Vx to Vx XOR Vy
					m_registers[register_x] ^= m_registers[register_y];
					m_program_counter += 2;
					break;
				case 0x0004: // 8XY4 - Add Vy to Vx, set Vf to 1 if there is a carry, and 0 otherwise
					m_registers[register_x] += m_registers[register_y];
					m_registers[0xF] = (m_registers[register_y] > (0xFF - m_registers[register_x])) ? 1 : 0;
					m_program_counter += 2;
					break;
				case 0x0005: // 8XY5 - Subtract Vy from Vx, set Vf to 0 if there is a borrow, and 1 otherwise
					m_registers[0xF] = (m_registers[register_y] > m_registers[register_x]) ? 0 : 1;
					m_registers[register_x] -= m_registers[register_y];
					m_program_counter += 2;
					break;
				case 0x0006: // 8XY6 - Store the least significant bit of Vx in Yf, and right shift Vx by 1
					m_registers[0xF] = m_registers[register_x] & 1;
					m_registers[register_x] >>= 1;
					m_program_counter += 2;
					break;
				case 0x0007: // 8XY7 - Set Vx to Vy - Vx, set Vf to 0 if there is a borrow, and 1 otherwise
					m_registers[0xF] = (m_registers[register_x] > m_registers[register_y]) ? 0 : 1;
					m_registers[register_x] = m_registers[register_y] - m_registers[register_x];
					m_program_counter += 2;
					break;
				case 0x000E: // 8XYE - Store the most significant bit of Vx in Vf, and left shift Vx by 1
					m_registers[0xF] = m_registers[register_x] >> 7;
					m_registers[register_x] <<= 1;
					m_program_counter += 2;
					break;
				}
				break;
			}
		case 0x9000: // 9XY0 - Skip next instruction if Vx =/= Vy
			{
				const uint8_t register_x = (instruction & 0x0F00) >> 8;
				const uint8_t register_y = (instruction & 0x00F0) >> 4;

				if (m_registers[register_x] != m_registers[register_y])
					m_program_counter += 2;

				m_program_counter += 2;
			}
			break;
		case 0xA000: // ANNN - Set the address register to NNN
			{
				const uint8_t value = instruction & 0x0FFF;

				m_memory[address_register] = value;

				m_program_counter += 2;
			}
		}

		if (m_delay_timer > 0)
			--m_delay_timer;

		if (m_sound_timer > 0)
			--m_sound_timer;

		return continue_running;
	}

	constexpr void clear_screen() noexcept
	{
		for (auto i = 0; i < display_memory_size; ++i)
			m_memory[display_memory_start + i] = 0;
	}

private:
	std::array<uint8_t, memory_size> m_memory{};
	std::array<uint8_t, 16> m_registers{};

	uint16_t m_program_counter = program_memory_start;
	uint16_t m_stack_pointer = call_stack_start;
	bool m_draw_flag = false;

	uint8_t m_delay_timer = 60;
	uint8_t m_sound_timer = 60;
};
