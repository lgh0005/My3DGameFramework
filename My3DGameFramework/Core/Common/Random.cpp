#include "CorePch.h"
#include "Random.h"

namespace MGF3D
{
	std::mt19937 Random::s_generator;

    void Random::Init(uint32 seed)
    {
        if (seed == 0)
        {
            // 하드웨어 엔트로피로 무작위 시딩
            std::random_device rd;
            s_generator.seed(rd());
        }
        else
        {
            // 전달받은 특정 시드로 고정 (디버깅/재현용)
            s_generator.seed(seed);
        }
    }

    int32 Random::GetInt(int32 min, int32 max)
    {
        std::uniform_int_distribution<int32> dist(min, max);
        return dist(s_generator);
    }

    float Random::GetFloat()
    {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(s_generator);
    }

    float Random::GetRange(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(s_generator);
    }

    bool Random::GetBool(float probability)
    {
        return GetFloat() < probability;
    }
}