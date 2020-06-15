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
		catch (const std::logic_error& e)
		{
			return std::nullopt;
		}
	}

private:
	std::unordered_map<std::string, std::string> m_config;

	template<typename T>
	inline T parse_value(const std::string& value) const
	{
		return static_cast<T>(value);
	}

	template<typename T>
	inline std::enable_if_t<std::is_floating_point_v<T>> parse_value(const std::string& value) const
	{
		return static_cast<T>(std::stold(value));
	}

	template<typename T>
	inline std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>> parse_value(const std::string& value) const
	{
		return static_cast<T>(std::stoll(value));
	}

	template<typename T>
	inline std::enable_if_t<std::is_integral_v<T>&& std::is_unsigned_v<T>> parse_value(const std::string& value) const
	{
		return static_cast<T>(std::stoull(value));
	}

	template<>
	inline bool parse_value<bool>(const std::string& value) const
	{
		if ((value != "false") && (value != "true"))
			throw std::invalid_argument("Value must is not \"true\" or \"false\"");

		return value == "true" ? true : false;
	}
};
