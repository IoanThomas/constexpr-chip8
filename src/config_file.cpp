#include "config_file.h"

#include <fstream>

config_file::config_file(const std::string& file_path)
{
	load(file_path);
}

void config_file::load(const std::string& file_path)
{
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		file.open(file_path);

		std::string line;
		while (std::getline(file, line))
		{
			const auto delimiter_pos = line.find('=');

			const auto key = line.substr(0, delimiter_pos);
			const auto value = line.substr(delimiter_pos + 1, line.length());

			m_config.insert({ key, value });
		}
	}
	catch (const std::ifstream::failure&)
	{
		throw std::runtime_error("Failed to open \"" + file_path + "\"");
	}
}
