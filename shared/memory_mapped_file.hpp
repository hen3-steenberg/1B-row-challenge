#ifndef MEMORY_MAPPED_FILE_DEFINITION
#define MEMORY_MAPPED_FILE_DEFINITION 1
#include <fileapi.h>
#include <handleapi.h>
#include <winbase.h>
#include <memoryapi.h>

#include <span>
#include <iterator>
#include <string_view>

#include "shared/path_literla.hpp"


struct memory_mapped_input_file
{
	HANDLE file_handle;
	HANDLE file_mapping;

	std::string_view file_content;

	template<size_t N>
	memory_mapped_input_file(path_literal<N> const& path)
	{
		file_handle = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		LARGE_INTEGER file_size;
		GetFileSizeEx(file_handle, &file_size);
		file_mapping = CreateFileMappingA(file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
		void* memory_address = MapViewOfFile(file_mapping, FILE_MAP_READ, 0, 0, 0);
		file_content = std::string_view { std::bit_cast<char*>(memory_address), file_size };
	}

	const char operator[](size_t idx) const& noexcept
	{
		return file_content[idx];
	}

	const auto begin() const& noexcept
	{
		return file_content.begin();
	}

	const auto end() const& noexcept
	{
		return file_content.end();
	}

	

	~memory_mapped_input_file()
	{
		UnmapViewOfFile(std::bit_cast<void*>(file_content.data()));
		CloseHandle(file_mapping);
		CloseHandle(file_handle);
	}

	struct line_reader
	{
		struct line_iterator
		{
			static constexpr bool is_whitespace(char c)
			{
				switch (c)
				{
				case ' ':
				case '\f':
				case '\n':
				case '\r':
				case '\t':
				case '\v':
					return true;
				default:
					return false;
				}
			}

			static constexpr bool is_line_end(char c)
			{
				switch (c)
				{
				case '\n':
				case '\r':
					return true;
				default:
					return false;
				}
			}
			using iterator_category = std::input_iterator_tag;
			using value_type = std::string_view;
			using difference_type = std::ptrdiff_t;
			using pointer = const std::string_view*;
			using reference = const std::string_view&;

			
			std::string_view buffer;
			std::string_view value;

			static constexpr std::string_view read_next_line(std::string_view & buffer)
			{
				std::string_view result{};
				size_t start_index = 0;
				//trim start
				while (is_whitespace(buffer[start_index])) 
				{
					++start_index;
					if (start_index >= buffer.size())
					{
						buffer = std::string_view{};
						return result;
					}
				}

				//get line
				size_t stop_index = start_index;
				while (!is_line_end(buffer[stop_index]))
				{
					if (stop_index >= buffer.size() - 1)
					{
						break;
					}
					++stop_index;
				}

				//trim end
				while (is_whitespace(buffer[stop_index]))
				{
					--stop_index;
				}
				++stop_index;
				size_t length = stop_index - start_index;
				std::string_view result = buffer.substr(start_index, length);

				buffer = buffer.substr(stop_index);

				return result;
			}

			constexpr line_iterator(std::span<const char> _buffer, size_t start)
				: buffer(_buffer), start_index(start), value(read_next_line(buffer, start_index))
			{}
		};
	};

	auto get_lines() const& noexcept
	{

	}

};

#endif