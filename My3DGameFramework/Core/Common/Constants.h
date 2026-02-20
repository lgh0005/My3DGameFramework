#pragma once

namespace MGF3D
{
	/*=====================================//
	//   default engine memory constants   //
	//=====================================*/
	static constexpr usize DEFAULT_ALIGNMENT = 16;

	static constexpr usize INITIAL_POOL_SIZE = 4 * 1024 * 1024;
	static constexpr usize SLAB_MAX_SIZE = 4096;
	static constexpr usize SLAB_BUCKET_COUNT = 9;

	/*===================================//
	//   default engine math constants   //
	//===================================*/
	constexpr float PI		   = glm::pi<float>();
	constexpr float HALF_PI	   = PI / 2.0f;
	constexpr float TWO_PI	   = PI * 2.0f;
	constexpr float DEG_TO_RAD = PI / 180.0f;
	constexpr float RAD_TO_DEG = 180.0f / PI;
	constexpr float EPSILON    = glm::epsilon<float>();
	constexpr float EPSILON2   = EPSILON * EPSILON;
}