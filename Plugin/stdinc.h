#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <vector>
#include <filesystem>
#include <fstream>
#include <functional>
#include <array>
#include <utility>
#include <unordered_map>
#include <string>
#include <iterator>
#include <algorithm>
#include <chrono>
#include <boost/utility/string_view.hpp>
#include <boost/algorithm/string.hpp>
#include "injector/hooking.hpp"
#include "injector/calling.hpp"

#define VALIDATE_SIZE(expr, size) static_assert(sizeof(expr) == size, "Type size error.");
