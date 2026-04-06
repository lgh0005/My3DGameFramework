#pragma once
#include <climits>
#include <cfloat>
#include <cstdint>

namespace MGF3D
{
	/*===================================//
	//    default engine math constants  //
	//===================================*/
	static constexpr float EPSILON = glm::epsilon<float>();
	static constexpr float EPSILON2 = EPSILON * EPSILON;
	static constexpr float PI = glm::pi<float>();
	static constexpr float HALF_PI = PI / 2.0f;
	static constexpr float TWO_PI = PI * 2.0f;
	static constexpr float DEG_TO_RAD = PI / 180.0f;
	static constexpr float RAD_TO_DEG = 180.0f / PI;

	/*=====================================//
	//     default engine numeric limits   //
	//=====================================*/
	// Floating Point
	static constexpr float MAX_FLOAT = FLT_MAX;
	static constexpr float MIN_FLOAT = FLT_MIN;
	static constexpr float LOWEST_FLOAT = -FLT_MAX;
	static constexpr double MAX_DOUBLE = DBL_MAX;
	static constexpr double MIN_DOUBLE = DBL_MIN;
	static constexpr double LOWEST_DOUBLE = -DBL_MAX;

	// Signed Integers
	static constexpr int8  MAX_INT8 = INT8_MAX;
	static constexpr int8  MIN_INT8 = INT8_MIN;
	static constexpr int16 MAX_INT16 = INT16_MAX;
	static constexpr int16 MIN_INT16 = INT16_MIN;
	static constexpr int32 MAX_INT32 = INT32_MAX;
	static constexpr int32 MIN_INT32 = INT32_MIN;
	static constexpr int64 MAX_INT64 = INT64_MAX;
	static constexpr int64 MIN_INT64 = INT64_MIN;

	// Unsigned Integers
	static constexpr uint8  MAX_UINT8 = UINT8_MAX;
	static constexpr uint16 MAX_UINT16 = UINT16_MAX;
	static constexpr uint32 MAX_UINT32 = UINT32_MAX;
	static constexpr uint64 MAX_UINT64 = UINT64_MAX;

	// Size & Address
	static constexpr usize   MAX_USIZE = UINTPTR_MAX;
	static constexpr uintptr MAX_UINTPTR = UINTPTR_MAX;
}