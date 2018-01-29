#pragma once
#include <fstream>
#include <cstddef>
#include <type_traits>
#include <filesystem>

class binary_file :public std::fstream
{
public:
	typedef std::fstream base_type;

	binary_file() = default;

	binary_file(const binary_file &) = delete;
	binary_file &operator=(const binary_file &) = delete;

	explicit binary_file::binary_file(const std::experimental::filesystem::v1::path &filename, std::ios::openmode mode)
		:base_type(filename, mode | std::ios::binary)
	{

	}

	binary_file(binary_file &&rv)
		:base_type(std::move(rv))
	{

	}

	template <typename T>
	std::enable_if_t<std::is_trivial_v<T>, binary_file &> read(T &result)
	{
		read(reinterpret_cast<char *>(&result), sizeof(T));
		return (*this);
	}

	binary_file &read(void *buffer, std::size_t size)
	{
		base_type::read(reinterpret_cast<char *>(buffer), size);
		return (*this);
	}

	template <typename T>
	std::enable_if_t<std::is_trivial_v<T>, T> read()
	{
		T result;

		read(result);

		return result;
	}

	template <typename T>
	std::enable_if_t<std::is_trivial_v<T>, binary_file &> write(const T &object)
	{
		write(reinterpret_cast<const char *>(&object), sizeof(T));
		return (*this);
	}

	binary_file &write(const void *buffer, std::size_t size)
	{
		base_type::write(reinterpret_cast<const char *>(buffer), size);
		return (*this);
	}

	template <typename container>
	std::enable_if_t<std::is_trivial_v<typename container::value_type>, binary_file &> write_container(const container &cont)
	{
		for (auto &element : cont)
		{
			write(element);
		}

		return (*this);
	}
};
