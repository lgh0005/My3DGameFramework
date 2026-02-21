#pragma once
#include <cstdint>
#include <string_view>

namespace MGF3D
{
	/*==========================//
	//   default engine types   //
	//==========================*/
	using int8   = std::int8_t;
	using int16  = std::int16_t;
	using int32  = std::int32_t;
	using int64  = std::int64_t;
	using uint8  = std::uint8_t;
	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;
	using uint64 = std::uint64_t;
	using usize  = std::size_t;

	using float32 = float;
	using float64 = double;
	
	/*====================================//
	//   default engine charactor types   //
	//====================================*/
	using char8  = char;     // 고정 8비트  (UTF-8용)
	using char16 = char16_t; // 고정 16비트 (UTF-16용)
	using char32 = char32_t; // 고정 32비트 (UTF-32용)

	/*=================================//
	//   default engine string types   //
	//=================================*/
	using cstr = const char*;
	using strview = std::string_view;

	/*==============================//
	//      Memory-related types    //
	//==============================*/
	using byte	    = uint8;
	using uintptr   = std::uintptr_t;
	using intptr    = std::intptr_t;
	using alignment = std::align_val_t;

	/*===============================//
	//   default engine math types   //
	//===============================*/
	using vec2   = glm::vec2;
	using vec3   = glm::vec3;
	using vec4   = glm::vec4;
	using mat3   = glm::mat3;
	using mat4   = glm::mat4;
	using quat   = glm::quat;
	using color  = vec4;
}