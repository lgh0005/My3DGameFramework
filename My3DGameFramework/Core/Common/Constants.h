#pragma once

namespace MGF3D
{
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