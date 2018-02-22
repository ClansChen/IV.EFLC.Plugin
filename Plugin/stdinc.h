#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <vector>
#include <filesystem>
#include <fstream>
#include "injector/hooking.hpp"
#include "injector/calling.hpp"

#define VALIDATE_SIZE(expr, size) static_assert(sizeof(expr) == size);
