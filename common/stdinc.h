#pragma once

#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <vector>
#include <filesystem>
#include <regex>
#include <fstream>
#include <functional>
#include <array>
#include <utility>
#include <map>
#include <set>
#include <unordered_map>
#include <string>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <boost/utility/string_view.hpp>
#include <boost/algorithm/string.hpp>
#include "injector/hooking.hpp"
#include "injector/calling.hpp"
#include "utf8cpp/utf8.h"
#include "BinaryFile.hpp"

#define FMT_HEADER_ONLY
#include "fmt/printf.h"

struct CharacterPos
{
    std::uint32_t row, column;
};

struct CharacterDataForReading
{
    std::uint32_t character;
    CharacterPos pos;
};

#define VALIDATE_SIZE(expr, size) static_assert(sizeof(expr) == size);
