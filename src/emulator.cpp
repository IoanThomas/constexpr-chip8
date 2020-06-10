#include "emulator.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

emulator::emulator(const std::string& rom_file_path)
	: m_window(sf::VideoMode(1280, 640), "CHIP-8")
{
	//m_window.setFramerateLimit(400);
	//m_window.setVerticalSyncEnabled(true);

	m_frame_texture.create(chip8::display_width, chip8::display_height);

	const auto scale_x = static_cast<float>(m_window.getSize().x) / chip8::display_width;
	const auto scale_y = static_cast<float>(m_window.getSize().y) / chip8::display_height;

	m_frame_sprite.setTexture(m_frame_texture);
	m_frame_sprite.setScale(scale_x, scale_y);

	load_rom(rom_file_path);
	generate_tone();
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
		{
			m_window.close();
		}
		else if (event.type == sf::Event::KeyPressed)
		{
			if ((event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::F)
				|| (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9))
			{
				m_chip8.key_press = event.key.code;
			}
		}
	}
}

void emulator::update()
{
	static float total_time = 0.0f;
	const auto delta = m_delta_clock.restart().asSeconds();

	if (total_time >= 1.0f)
	{
		std::cout << "FPS: " << (1.0f / delta) << "\n";
		total_time = 0.0f;
	}

	if (m_running)
	{
		if (!m_chip8.next_instruction())
			m_running = false;

		if (m_chip8.sound_timer != 0)
			m_tone.play();
	}

	total_time += delta;
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
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		file.open(rom_file_path, std::ios::binary);

		uint16_t count = 0;
		std::for_each(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), [this, &count](const char c) {
			m_chip8.memory[chip8::program_memory_start + count] = static_cast<uint8_t>(c);
			++count;
		});
	}
	catch (const std::ifstream::failure&)
	{
		throw std::runtime_error("Failed to open \"" + rom_file_path + "\"");
	}
}

void emulator::generate_tone()
{
	constexpr unsigned int samples = 4410;
	constexpr unsigned int sample_rate = 44100;
	constexpr unsigned int amplitude = 30000;
	constexpr float increment = 440.0f / 44100.0f;

	sf::Int16 raw[samples];

	float x = 0.0f;
	for (auto i = 0; i < samples; ++i)
	{
		raw[i] = static_cast<sf::Int16>(amplitude * std::sin(x * (std::atan(1.0) * 4.0) * 2.0));
		x += increment;
	}

	m_sound_buffer.loadFromSamples(raw, samples, 1, sample_rate);
	m_tone.setBuffer(m_sound_buffer);
}
