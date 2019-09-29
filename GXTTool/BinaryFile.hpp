#pragma once
#include <cstddef>
#include <filesystem>
#include <cstdio>
#include <type_traits>
#include <vector>

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

    BinaryFile(const std::filesystem::path &filename, OpenMode method)
    {
        Open(filename, method);
    }

    bool Open(const std::filesystem::path &filename, OpenMode method)
    {
        const char *method_str;

        Close();

        switch (method)
        {
        case OpenMode::ReadOnly:
            method_str = "rb";
            break;

        case OpenMode::WriteOnly:
            method_str = "wb";
            break;

        case OpenMode::ReadWrite:
            method_str = "rb+";
            break;

        default:
            return false;
        }

        m_pFile = std::fopen(filename.string().c_str(), method_str);
        return (m_pFile != nullptr);
    }

    void Close()
    {
        if (m_pFile != nullptr)
        {
            std::fclose(m_pFile);
            m_pFile = nullptr;
        }
    }

    bool Opened() const
    {
        return m_pFile != nullptr;
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
        case SeekMode::Begin:
            temp = SEEK_SET;
            break;

        case SeekMode::Current:
            temp = SEEK_CUR;
            break;

        case SeekMode::End:
            temp = SEEK_END;
            break;

        default:
            return *this;
        }

        _fseeki64(m_pFile, offset, temp);

        return *this;
    }

    std::int64_t Tell() const
    {
        return _ftelli64(m_pFile);
    }

    BinaryFile &Read(void *buffer, std::size_t size)
    {
        std::fread(buffer, size, 1, m_pFile);
        return *this;
    }

    template <typename T>
    std::enable_if_t<std::is_pod_v<T>, BinaryFile &> Read(T &object)
    {
        Read(&object, sizeof(object));
        return *this;
    }

    template <typename T>
    std::enable_if_t<std::is_pod_v<T>, BinaryFile &> ReadArray(std::size_t count, std::vector<T> &objects)
    {
        objects.resize(count);
        Read(objects.data(), sizeof(T) * count);
        return *this;
    }

    BinaryFile &Write(const void *buffer, std::size_t size)
    {
        std::fwrite(buffer, size, 1, m_pFile);
        return *this;
    }

    template <typename T>
    std::enable_if_t<std::is_pod_v<T>, BinaryFile &> Write(const T &object)
    {
        Write(&object, sizeof(object));
        return *this;
    }

    template <typename T>
    std::enable_if_t<std::is_pod_v<T>, BinaryFile &> WriteArray(const std::vector<T> &objects)
    {
        Write(objects.data(), sizeof(T) * objects.size());
        return *this;
    }

private:
    std::FILE * m_pFile = nullptr;
};
