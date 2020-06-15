#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>

class config_file
{
public:
	config_file() = default;
	config_file(const std::string& file_path);

	void load(const std::string& file_path);

	template<typename T>
	inline std::optional<T> get_value(const std::string& key) const
	{
		try
		{
			return parse_value<T>(m_config.at(key));
		}
		catch (const std::logic_error&)
		{
			return std::nullopt;
		}
	}

private:
	std::unordered_map<std::string, std::string> m_config;

	template<typename T>
	inline T parse_value(const std::string& value) const
	{
		if constexpr (std::is_floating_point_v<T>)
		{
			return static_cast<T>(std::stold(value));
		}
		else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>)
		{
			return static_cast<T>(std::stoll(value));
		}
		else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>)
		{
			return static_cast<T>(std::stoull(value));
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			if ((value != "false") && (value != "true"))
				throw std::invalid_argument("Value must be \"true\" or \"false\"");

			return value == "true" ? true : false;
		}
		else
		{
			return static_cast<T>(value);
		}
	}
};
