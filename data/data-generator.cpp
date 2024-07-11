#include "file_locations.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <random>
#include <print>

constexpr static auto data_dir = get_data_dir();
constexpr static auto test_data_path = get_test_data_path();
constexpr static auto data_path = get_data_path();
constexpr static auto station_list_path = get_stationlist_path();

void generate_station_list()
{
	std::ifstream test_data{ test_data_path.to_path() };
	std::ofstream station_list{ station_list_path.to_path() };

	for (std::string line; test_data.good(); std::getline(test_data, line))
	{
		if (line.empty() || line.starts_with('#')) continue;
		std::string_view line_view = line;
		size_t station_length = line_view.find(';');
		std::string_view station = line_view.substr(0, station_length);
		station_list << station << '\n';
	}
}

std::vector<std::string> read_station_list()
{
	std::vector<std::string> stations;
	std::ifstream station_read_file{ station_list_path.to_path() };
	while (station_read_file.good())
	{
		std::string station;
		std::getline(station_read_file, station);
		if (!station.empty())
		{
			stations.emplace_back(std::move(station));
		}
	}
	return stations;
}

void sort_station_list()
{
	std::vector<std::string> stations = read_station_list();

	std::ranges::sort(stations);

	std::ofstream station_write_file{ station_list_path.to_path(), std::ios::trunc };

	bool first = true;
	for (auto const& station : stations)
	{
		if (first)
		{
			first = false;
		}
		else
		{
			station_write_file << '\n';
		}
		station_write_file << station;
	}
	
}


void generate_data()
{
	std::vector<std::string> stations = read_station_list();
	std::ofstream data_file{ data_path.to_path(), std::ios::trunc };
	std::random_device rd;
	std::mt19937 gen{ rd() };
	std::uniform_int_distribution<size_t> station_select{ 0, stations.size() - 1 };
	std::uniform_real_distribution<float> value_generator{ -99.9, 99.9 };

	std::cout << "Generating 1 billion rows of data.\n";
	std::cout << "Generated 0 lines of data or 0%";

	for (size_t chunck = 0; chunck < 100; ++chunck)
	{
		for (size_t row = 0; row < 10000000; ++row)
		{
			std::println(data_file, "{};{}", stations[station_select(gen)], value_generator(gen));
		}
		data_file.flush();
		size_t percentage = chunck + 1;
		std::cout << "\rGenerated " << percentage * 10000000ull << " lines of data or " << percentage << "%";
	}
}


int main()
{
	
	

	if (!std::filesystem::exists(station_list_path))
	{
		generate_station_list();
	}
	sort_station_list();

	generate_data();
	return 0;
}