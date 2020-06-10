#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "chip8/chip8.h"

class emulator
{
public:
	emulator(const std::string& rom_file_path);

	void run();

private:
	sf::RenderWindow m_window;
	sf::Clock m_delta_clock;
	chip8 m_chip8;
	bool m_running = true;

	void handle_events();
	void update();
	void render();
	void load_rom(const std::string& rom_file_path);
};
