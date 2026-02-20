#pragma once
#include <climits>
#include <cfloat>
#include <cstdint>

namespace MGF3D
{
	/*=====================================//
	//    default engine memory constants  //
	//=====================================*/
	static constexpr usize DefaultAlignment = 16;
	static constexpr usize InitialPoolSize = 4 * 1024 * 1024;
	static constexpr usize SlabMaxSize = 4096;
	static constexpr usize SlabBucketCount = 9;

	/*===================================//
	//    default engine math constants  //
	//===================================*/
	static constexpr float Pi = glm::pi<float>();
	static constexpr float HalfPi = Pi / 2.0f;
	static constexpr float TwoPi = Pi * 2.0f;
	static constexpr float DegToRad = Pi / 180.0f;
	static constexpr float RadToDeg = 180.0f / Pi; // 내부 변수명 변경 시 참조 주의
	static constexpr float Epsilon = glm::epsilon<float>();
	static constexpr float Epsilon2 = Epsilon * Epsilon;

	/*=====================================//
	//     default engine numeric limits   //
	//=====================================*/
	// Floating Point
	static constexpr float FloatMax = FLT_MAX;
	static constexpr float FloatMin = FLT_MIN;
	static constexpr float FloatLowest = -FLT_MAX;
	static constexpr double DoubleMax = DBL_MAX;
	static constexpr double DoubleMin = DBL_MIN;
	static constexpr double DoubleLowest = -DBL_MAX;

	// Signed Integers
	static constexpr int8  MaxI8 = INT8_MAX;
	static constexpr int8  MinI8 = INT8_MIN;
	static constexpr int16 MaxI16 = INT16_MAX;
	static constexpr int16 MinI16 = INT16_MIN;
	static constexpr int32 MaxI32 = INT32_MAX;
	static constexpr int32 MinI32 = INT32_MIN;
	static constexpr int64 MaxI64 = INT64_MAX;
	static constexpr int64 MinI64 = INT64_MIN;

	// Unsigned Integers
	static constexpr uint8  MaxU8 = UINT8_MAX;
	static constexpr uint16 MaxU16 = UINT16_MAX;
	static constexpr uint32 MaxU32 = UINT32_MAX;
	static constexpr uint64 MaxU64 = UINT64_MAX;

	// Size & Address
	static constexpr usize   MaxUSize = UINTPTR_MAX;
	static constexpr uintptr MaxUIntPtr = UINTPTR_MAX;
}