#pragma once
#include <random>

namespace MGF3D
{
	class Random
	{
		DECLARE_UTILITIES(Random)

	public:
		static void Init(uint32 seed = 0);

	public:
		static int32 GetInt(int32 min, int32 max);
		static float GetFloat();
		static float GetRange(float min, float max);
		static bool  GetBool(float probability = 0.5f);

	private:
		static std::mt19937 s_generator;
	};
}