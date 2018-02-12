#pragma once
#include <cstddef>
#include <filesystem>
#include <cstdio>
#include <type_traits>
#include <memory>
#include <vector>

struct FilePtrDeleter
{
	void operator()(std::FILE *file) const
	{
		std::fclose(file);
	}
};

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

	BinaryFile(const std::experimental::filesystem::v1::path &filename, OpenMode method)
	{
		Open(filename, method);
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

		m_pFile.reset(std::fopen(filename.string().c_str(), method_str));
	}

	void Close()
	{
		m_pFile.reset();
	}

	bool Opened() const
	{
		return (bool)m_pFile;
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

		_fseeki64(m_pFile.get(), offset, temp);

		return *this;
	}

	std::int64_t Tell() const
	{
		return _ftelli64(m_pFile.get());
	}

	BinaryFile &Read(void *buffer, std::size_t size)
	{
		std::fread(buffer, size, 1, m_pFile.get());
		return *this;
	}

	template <typename T>
	std::enable_if_t<std::is_trivial_v<T>, BinaryFile &> Read(T &object)
	{
		Read(&object, sizeof(object));
		return *this;
	}

	template <typename T>
	std::enable_if_t<std::is_trivial_v<T>, BinaryFile &> ReadArray(std::size_t count, std::vector<T> &objects)
	{
		objects.resize(count);
		Read(objects.data(), sizeof(T) * count);
		return *this;
	}

	BinaryFile &Write(const void *buffer, std::size_t size)
	{
		std::fwrite(buffer, size, 1, m_pFile.get());
		return *this;
	}

	template <typename T>
	std::enable_if_t<std::is_trivial_v<T>, BinaryFile &> Write(const T &object)
	{
		Write(&object, sizeof(object));
		return *this;
	}

	template <typename T>
	std::enable_if_t<std::is_trivial_v<T>, BinaryFile &> WriteArray(const std::vector<T> &objects)
	{
		
		Write(objects.data(), sizeof(T) * objects.size());
		return *this;
	}

private:
	std::unique_ptr<std::FILE, FilePtrDeleter> m_pFile;
};
