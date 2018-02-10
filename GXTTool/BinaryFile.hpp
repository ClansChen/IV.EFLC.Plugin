#pragma once
#include <cstddef>
#include <filesystem>
#include <cstdio>
#include <type_traits>

class BinaryFile
{
public:
	enum class OpenMode
	{
		ReadOnly,
		WriteOnly,
		ReadWrite
	};

	enum class SeekMode
	{
		Begin,
		Current,
		End
	};

	BinaryFile() = default;
	BinaryFile(BinaryFile &) = delete;
	BinaryFile &operator=(BinaryFile &) = delete;

	BinaryFile(const std::experimental::filesystem::v1::path &filename, OpenMode method)
	{
		Open(filename, method);
	}

	BinaryFile(BinaryFile &&rvalue)
	{
		m_File = rvalue.m_File;

		rvalue.m_File = nullptr;
	}

	BinaryFile &operator=(BinaryFile &&rvalue)
	{
		Close();

		m_File = rvalue.m_File;

		rvalue.m_File = nullptr;

		return *this;
	}

	~BinaryFile()
	{
		Close();
	}

	void Open(const std::experimental::filesystem::v1::path &filename, OpenMode method)
	{
		Close();

		const char *method_str;

		switch (method)
		{
		case BinaryFile::OpenMode::ReadOnly:
			method_str = "rb";
			break;

		case BinaryFile::OpenMode::WriteOnly:
			method_str = "wb";
			break;

		case OpenMode::ReadWrite:
			method_str = "rb+";
			break;

		default:
			return;
		}

		m_File = std::fopen(filename.string().c_str(), method_str);
	}

	void Close()
	{
		if (m_File)
		{
			std::fclose(m_File);
		}

		m_File = nullptr;
	}

	bool Opened() const
	{
		return (m_File == nullptr);
	}

	operator bool() const
	{
		return Opened();
	}

	BinaryFile &Seek(std::int64_t offset, SeekMode mode)
	{
		int temp;

		switch (mode)
		{
		case BinaryFile::SeekMode::Begin:
			temp = SEEK_SET;
			break;

		case BinaryFile::SeekMode::Current:
			temp = SEEK_CUR;
			break;

		case BinaryFile::SeekMode::End:
			temp = SEEK_END;
			break;

		default:
			return *this;
		}

		_fseeki64(m_File, offset, temp);

		return *this;
	}

	std::int64_t Tell() const
	{
		return _ftelli64(m_File);
	}

	BinaryFile &Read(void *buffer, std::uint64_t size)
	{
		std::fread(buffer, size, 1, m_File);

		return *this;
	}

	template <typename T>
	std::enable_if_t<std::is_trivial_v<T>, BinaryFile &> Read(T &object)
	{
		Read(&object, sizeof(object));
	}

	BinaryFile &Write(const void *buffer, std::uint64_t size)
	{
		std::fwrite(buffer, size, 1, m_File);

		return *this;
	}

	template <typename T>
	std::enable_if_t<std::is_trivial_v<T>, BinaryFile &> Write(const T &object)
	{
		Write(&object, sizeof(object));

		return *this;
	}

	template <typename T>
	std::enable_if_t<std::is_trivial_v<typename T::value_type>, BinaryFile &> WriteContainer(const T &objects)
	{
		for (auto &object : objects)
		{
			Write(&object, sizeof(object));
		}

		return *this;
	}

private:
	FILE * m_File = nullptr;
};

