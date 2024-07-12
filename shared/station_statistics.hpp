#ifndef STATION_STATISTICS_DEFINITION
#define STATION_STATISTICS_DEFINITION 1
#include <cstdint>
#include <fstream>
#include "data/file_locations.hpp"
#include <string>
#include <iterator>
#include <ios>

struct station_statistics
{
	uint64_t count;
	float min;
	float max;
	float sum;

	constexpr station_statistics(float value) noexcept
		: count(1), min(value), max(value), sum(value)
	{}

	constexpr float get_average() const& noexcept
	{
		return sum / count;
	}

	constexpr void update(float value) & noexcept
	{
		count++;
		sum += value;
		if (value < min)
		{
			min = value;
		}
		if (value > max)
		{
			max = value;
		}
	}
};

struct simple_file_reader
{

	struct line
	{
		std::string _line;

		constexpr bool should_process() const& noexcept
		{
			return !(_line.empty() || _line.starts_with('#'));
		}

		constexpr std::string_view to_view() const&
		{
			return std::string_view{ _line };
		}
	};
	std::ifstream file;

	template<size_t N>
	simple_file_reader(path_literal<N> const& path)
		: file(path.to_path())
	{}

	auto begin()&
	{
		return std::istream_iterator<line> {file};
	}

	constexpr auto end() & noexcept
	{
		return std::istream_iterator<line> {std::default_sentinel};
	}
};

std::istream& operator>>(std::istream& is, simple_file_reader::line& l)
{
	return std::getline(is >> std::noskipws, l._line);
}

float to_float(std::string_view str)
{
	char* end = nullptr;
	return std::strtof(str.data(), &end);
}

#endif