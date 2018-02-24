//Core code from Hooking.Patterns
//https://github.com/ThirteenAG/Hooking.Patterns

#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <utility>

extern HMODULE pattern_default_module;

class memory_pointer
{
    union
    {
        void *_pointer;
        std::uintptr_t _address;
    };

public:
    memory_pointer()
        :_pointer{}
    {

    }

    memory_pointer(void *pointer)
        : _pointer(pointer)
    {
    }

    memory_pointer(std::uintptr_t address)
        : _address(address)
    {
    }

    memory_pointer(const memory_pointer &rhs) = default;

    std::uintptr_t integer(std::ptrdiff_t offset = 0) const
    {
        return (this->_address + offset);
    }

    template<typename T = void>
    T *pointer(std::ptrdiff_t offset = 0) const
    {
        return reinterpret_cast<T *>(this->integer(offset));
    }

    bool operator==(const memory_pointer &rhs) const
    {
        return this->integer() == rhs.integer();
    }

    bool operator!=(const memory_pointer &rhs) const
    {
        return this->integer() != rhs.integer();
    }

    bool operator>(const memory_pointer &rhs) const
    {
        return this->integer() > rhs.integer();
    }

    bool operator<(const memory_pointer &rhs) const
    {
        return this->integer() < rhs.integer();
    }

    bool operator>=(const memory_pointer &rhs) const
    {
        return this->integer() >= rhs.integer();
    }

    bool operator<=(const memory_pointer &rhs) const
    {
        return this->integer() <= rhs.integer();
    }

    memory_pointer operator+(std::ptrdiff_t offset) const
    {
        return memory_pointer{ this->integer(offset) };
    }

    memory_pointer operator-(std::ptrdiff_t offset) const
    {
        return memory_pointer{ this->integer(-offset) };
    }

    memory_pointer &operator+=(std::ptrdiff_t offset)
    {
        this->_address += offset;
        return *this;
    }

    memory_pointer &operator-=(std::ptrdiff_t offset)
    {
        this->_address -= offset;
        return *this;
    }

    operator std::uintptr_t() const
    {
        return this->integer();
    }

    template <typename T>
    operator T*() const
    {
        return this->raw<T>();
    }
};

class byte_pattern
{
    std::vector<std::pair<std::uintptr_t, std::uintptr_t>> _ranges;
    std::vector<std::uint8_t> _pattern;
    std::vector<std::uint8_t> _mask;
    std::vector<memory_pointer> _results;
    std::string _literal;

    std::ptrdiff_t _bmbc[256];

    void transform_pattern(const char *pattern_literal);
    void get_module_ranges(memory_pointer module);

    void bm_preprocess();
    void bm_search();

    void debug_output() const;

public:
    byte_pattern();

    byte_pattern &set_pattern(const char *pattern_literal);
    byte_pattern &set_pattern(const void *data, std::size_t size);

    byte_pattern &set_module();
    byte_pattern &set_module(memory_pointer module);
    byte_pattern &set_range(memory_pointer beg, memory_pointer end);
    byte_pattern &search();

    byte_pattern &find_pattern(const char *pattern_literal);
    memory_pointer find_first(const char *pattern_literal);
    byte_pattern &find_pattern(const void *data, std::size_t size);
    memory_pointer find_first(const void *data, std::size_t size);

    memory_pointer get(std::size_t index) const;
    memory_pointer get_first() const;

    std::size_t count() const;
    bool has_size(std::size_t expected) const;
    bool empty() const;
    void clear();

    template <typename Fn>
    void for_each_result(Fn Pr) const
    {
        for (auto &result : this->_results)
        {
            Pr(result);
        }
    }
};

extern byte_pattern g_pattern;
