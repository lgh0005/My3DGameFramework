#include "CorePch.h"
#include "SpatialAxes.h"

namespace MGF3D
{
	SpatialAxes::SpatialAxes() = default;

	void SpatialAxes::Update(const quat& rotation)
	{
		// 1. 전방 벡터
		m_forward = Math::Normalize(rotation * vec3(0.0f, 0.0f, -1.0f));

		// 2. 상방 벡터
		m_up = Math::Normalize(rotation * vec3(0.0f, 1.0f, 0.0f));

		// 3. 우측 벡터
		m_right = Math::Normalize(rotation * vec3(1.0f, 0.0f, 0.0f));
	}

	const vec3& SpatialAxes::Forward() const
	{
		return m_forward;
	}

	const vec3& SpatialAxes::Up() const
	{
		return m_up;
	}

	const vec3& SpatialAxes::Right() const
	{
		return m_right;
	}
}