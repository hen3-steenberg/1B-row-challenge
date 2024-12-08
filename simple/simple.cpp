#include "shared/station_statistics.hpp"
#include "data/file_locations.hpp"
#include "shared/memory_mapped_file.hpp"

#include <map>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <iterator>

struct station_list
{
	struct station
	{
		std::string name;
		station_statistics stats;

		station(std::string_view _name, float val)
			: name(_name), stats(val)
		{}

		void update(float val)&
		{
			stats.update(val);
		}

		std::string_view get_name() const&
		{
			return std::string_view{ name };
		}
	};
	std::map<std::string_view, station> data;

	void update(std::string_view _station, float value)
	{
		if (data.contains(_station))
		{
			data.at(_station).update(value);
		}
		else
		{
			station stat{ _station, value };
			data.insert(std::make_pair(stat.get_name(), std::move(stat)));
		}
	}

	auto begin()& noexcept
	{
		return data.begin();
	}

	auto end()& noexcept
	{
		return data.end();
	}

	auto begin() const& noexcept
	{
		return data.cbegin();
	}

	auto end() const& noexcept
	{
		return data.cend();
	}
};



station_list read_data()
{
	station_list result;
	simple_file_reader data_file{ get_data_path() };

	std::cout << "Reading file\n";

	size_t counter = 0;
	for (auto line : data_file)
	{
		if (line.should_process())
		{
			++counter;
			std::string_view _line = line.to_view();
			size_t name_length = _line.find(';');
			std::string_view station = _line.substr(0, name_length);
			std::string_view string_value = _line.substr(name_length + 1);
			float value = to_float(string_value);
			result.update(station, value);
		}

		if (counter % 1000000 == 0)
		{
			float percent = counter / 10000000.0f;
			std::cout << "\r\t" << percent << "% complete\t\t\t";
		}
		
	}

	return result;
}

int main()
{
	auto data = read_data();
	std::cout << "\nDisplaying statistics\n";
	for (auto const& [name, station] : data)
	{
		std::cout << "Name                : " << name << '\n';
		std::cout << "  Number of entries : " << station.stats.count << '\n';
		std::cout << "  Maximum Temp      : " << station.stats.max << '\n';
		std::cout << "  Minimum Temp      : " << station.stats.min << '\n';
		std::cout << "  Average Temp      : " << station.stats.get_average() << "\n\n";
	}
	return 0;
}