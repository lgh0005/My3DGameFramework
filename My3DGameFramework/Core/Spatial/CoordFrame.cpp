#include "CorePch.h"
#include "CoordFrame.h"
#include "Spatial/Position.h"
#include "Spatial/Rotation.h"
#include "Spatial/Scale.h"

namespace MGF3D
{
	CoordFrame::CoordFrame()
	{
		SetCoordDirty();
	}

	void CoordFrame::SetCoordDirty()
	{
		m_isCoordDirty = true;
		m_isInverseDirty = true;
	}

	void CoordFrame::Update(const Position& pos, const Rotation& rot, const Scale& scale)
	{
		if (!m_isCoordDirty) return;

		// M = T * R * S
		m_modelMatrix = glm::translate(mat4(1.0f), pos.AsVec3());
		m_modelMatrix *= glm::mat4_cast(rot.value);
		m_modelMatrix = glm::scale(m_modelMatrix, scale.AsVec3());

		m_isCoordDirty = false;
	}

	const mat4& CoordFrame::GetModelMatrix() const
	{
		return m_modelMatrix;
	}

	const mat4& CoordFrame::GetInverseMatrix() const
	{
		if (m_isInverseDirty)
		{
			m_inverseMatrix = Math::Inverse(m_modelMatrix);
			m_isInverseDirty = false;
		}

		return m_inverseMatrix;
	}

	vec3 CoordFrame::ToWorldPoint(const vec3& localPoint) const
	{
		return vec3(m_modelMatrix * vec4(localPoint, 1.0f));
	}

	vec3 CoordFrame::ToLocalPoint(const vec3& worldPoint) const
	{
		return vec3(GetInverseMatrix() * vec4(worldPoint, 1.0f));
	}

	vec3 CoordFrame::ToWorldDir(const vec3& localDir) const
	{
		return Math::Normalize(vec3(m_modelMatrix * vec4(localDir, 0.0f)));
	}

	vec3 CoordFrame::ToLocalDir(const vec3& worldDir) const
	{
		return Math::Normalize(vec3(GetInverseMatrix() * vec4(worldDir, 0.0f)));
	}
}