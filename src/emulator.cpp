#include "emulator.h"

#include <algorithm>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <vector>

emulator::emulator(const std::string& rom_file_path)
	: m_window(sf::VideoMode(1280, 640), "CHIP-8")
{
	//m_window.setVerticalSyncEnabled(true);

	m_frame_texture.create(chip8::display_width, chip8::display_height);

	m_frame_sprite.setTexture(m_frame_texture);
	m_frame_sprite.setScale(20.0f, 20.0f);

	load_rom(rom_file_path);
}

void emulator::run()
{
	while (m_window.isOpen())
	{
		handle_events();
		update();
		render();
	}
}

void emulator::handle_events()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			m_window.close();
	}
}

void emulator::update()
{
	const auto delta = m_delta_clock.restart().asSeconds();

	std::cout << "FPS: " << (1.0f / delta) << "\n";

	if (m_running)
	{
		if (!m_chip8.next_instruction())
			m_running = false;
	}
}

void emulator::render()
{
	if (m_chip8.draw_flag)
	{
		m_window.clear();

		sf::Image frame;
		frame.create(chip8::display_width, chip8::display_height);

		for (auto y = 0; y < chip8::display_height; ++y)
		{
			for (auto x = 0; x < chip8::display_width; ++x)
			{
				frame.setPixel(x, y, m_chip8.is_pixel_set(x, y) ? sf::Color::White : sf::Color::Transparent);
			}
		}

		m_frame_texture.update(frame);
		m_window.draw(m_frame_sprite);
	}

	m_window.display();
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
