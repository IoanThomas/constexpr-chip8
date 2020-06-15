#pragma once

#include <string>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include "chip8.h"

class emulator
{
public:
	emulator(const std::string& rom_file_path);

	void run();

private:
	sf::RenderWindow m_window;
	sf::Clock m_delta_clock;

	sf::Texture m_frame_texture;
	sf::Sprite m_frame_sprite;
	sf::SoundBuffer m_sound_buffer;

	chip8 m_chip8;
	sf::Sound m_tone;
	bool m_running = true;

	void handle_events();
	void update();
	void render();

	void load_config();
	void load_rom(const std::string& rom_file_path);
	void generate_tone();
	void create_sprite();
};
