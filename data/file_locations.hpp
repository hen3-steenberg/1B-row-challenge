#ifndef FILE_LOCATION_DEFINITION
#define FILE_LOCATION_DEFINITION 1
#include <source_location>
#include <array>
#include <filesystem>
#include <iostream>

template<size_t N>
struct path_literal
{
	std::array<char, N> data;
	static constexpr char windows_separator = '\\';
	consteval path_literal(const char* str) noexcept
		: data()
	{
		for (size_t idx = 0; idx < N - 1; ++idx)
		{
			if (str[idx] == windows_separator)
			{
				data[idx] = '/';
			}
			else
			{
				data[idx] = str[idx];
			}
		}
		data[N - 1] = '\0';
	}

	template<size_t A, size_t B>
	consteval path_literal(std::array<char, A> const& a, std::array<char, B> const& b)
		: data()
	{
		static_assert(A + B >= N);
		size_t idx = 0;
		for (; idx < A - 1; ++idx)
		{
			data[idx] = a[idx];
		}
		data[idx] = '/';
		++idx;
		for (; idx < N - 1; ++idx)
		{
			data[idx] = b[idx - A];
		}
		data[N - 1] = '\0';
	}

	consteval size_t size() const& noexcept
	{
		return N;
	}

	constexpr size_t get_parent_length() const& noexcept
	{
		for (size_t idx = N; idx > 0; --idx)
		{
			if (data[idx - 1] == '/') return idx;
		}
		return 0;
	}

	template<size_t M>
	consteval path_literal<N + M>  operator/(path_literal<M> const& other) const & noexcept
	{
		return path_literal<N + M> {data, other.data};	
	}

	template<>
	consteval path_literal<N>  operator/(path_literal<0> const& other) const& noexcept
	{
		return path_literal<N> { data.data() };
	}

	constexpr const char* c_str() const& noexcept
	{
		return data.data();
	}

	std::filesystem::path to_path() const&
	{
		return std::filesystem::path{ c_str() };
	}

	operator std::filesystem::path() const&
	{
		return to_path();
	}
	
};

template<>
struct path_literal<0>
{
	std::array<char, 0> data;
	consteval path_literal(const char* str) noexcept
		: data() {}

	consteval size_t size() const& noexcept
	{
		return 0;
	}

	constexpr size_t get_parent_length() const& noexcept
	{
		return 0;
	}

	consteval const char* c_str() const& noexcept
	{
		return "";
	}
};

template<size_t N>
std::ostream& operator<<(std::ostream& os, path_literal<N> const& path)
{
	os << path.c_str();
	return os;
}

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