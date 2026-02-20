#pragma once

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

	/*==============================//
	//      Memory-related types    //
	//==============================*/
	using byte	  = uint8;
	using uintptr = std::uintptr_t;
	using intptr  = std::intptr_t;

	/*=================================//
	//   default engine string types   //
	//=================================*/
	using cstr = const char*;
	using strview = std::string_view;

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

	/*=================================//
	//   default engine thread tools   //
	//=================================*/
	using ThreadMutex = std::mutex;
	using ScopedLock = std::lock_guard<std::mutex>;
}