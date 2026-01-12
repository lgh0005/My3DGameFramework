#pragma once

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using usize = size_t;

using ThreadMutex = std::mutex;
using ScopedLock = std::lock_guard<std::mutex>;

constexpr float epsilon = glm::epsilon<float>();
constexpr float epsilon2 = epsilon * epsilon;
constexpr float pi = glm::pi<float>();
constexpr float halfPi = pi / 2.0f;
constexpr float twoPi = pi * 2.0f;
constexpr float degToRad = pi / 180.0f;           
constexpr float radToDeg = 180.0f / pi;

using ScriptID = uint64;