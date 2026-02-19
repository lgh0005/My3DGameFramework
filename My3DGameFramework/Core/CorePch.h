#pragma once

/*============================//
//   C++ standard libraries   //
//============================*/
// C standard library wrappers
#include <cstdint>
#include <climits>
#include <cmath>
#include <cstdarg>

// C++ strings
#include <string>
#include <string_view>

// C++ containers
#include <vector>
#include <array>
#include <unordered_map>

// C++ memories
#include <memory>
#include <memory_resource>
namespace pmr = std::pmr;

// C++ templates
#include <type_traits>

// C++ thread
#include <thread>
#include <mutex>

// C++ utilities
#include <random>
#include <optional>
#include <algorithm>
#include <functional>

// C++ filesystem
#include <filesystem>
#include <fstream>
#include <sstream>
namespace fs = std::filesystem;

/*===========================//
//   Engine common headers   //
//===========================*/
#include "Common/Defines.h"
#include "Common/Enums.h"
#include "Common/Types.h"
#include "Common/Constants.h"