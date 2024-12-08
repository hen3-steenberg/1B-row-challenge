#ifndef FILE_LOCATION_DEFINITION
#define FILE_LOCATION_DEFINITION 1
#include <source_location>
#include "shared/path_literla.hpp"



constexpr size_t str_length(const char* str)
{
	size_t length = 0;
	while (str[length++]) {};
	return length;
}

template<path_literal path>
consteval auto get_parent() noexcept
{
	constexpr auto parent_length = path.get_parent_length();
	return path_literal<parent_length> {path.data.data()};
}


consteval auto get_src_file()
{
	constexpr auto src_location = std::source_location::current().file_name();
	constexpr size_t src_size = str_length(src_location);
	return path_literal<src_size> { src_location };
}

consteval auto get_data_dir()
{
	constexpr path_literal src_location = get_src_file();
	return get_parent<src_location>();
}

consteval auto get_test_data_path()
{
	return get_data_dir() / path_literal<14>("test-data.csv");
}

consteval auto get_data_path()
{
	return get_data_dir() / path_literal<9>("data.csv");
}

consteval auto get_stationlist_path()
{
	return get_data_dir() / path_literal<16>("stationlist.txt");
}


#endif