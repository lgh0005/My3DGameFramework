#include "CorePch.h"
#include "ViewProjection.h"

namespace MGF3D
{
	ViewProjection::ViewProjection() = default;

	void ViewProjection::UpdateView(const mat4& cameraWorldMatrix)
	{
		m_view = glm::inverse(cameraWorldMatrix);
		m_isVpDirty = true;
		m_isInvVpDirty = true;
	}

	void ViewProjection::SetPerspective(float fovDeg, float aspect, float nearZ, float farZ)
	{
		m_proj = glm::perspective(Math::ToRadians(fovDeg), aspect, nearZ, farZ);
		m_isVpDirty = true;
		m_isInvVpDirty = true;
	}

	const mat4& ViewProjection::GetVP() const
	{
		if (m_isVpDirty)
		{
			m_viewProj = m_proj * m_view;
			m_isVpDirty = false;
		}

		return m_viewProj;
	}

	const mat4& ViewProjection::GetInvVP() const
	{
		if (m_isInvVpDirty)
		{
			m_invViewProj = glm::inverse(GetVP());
			m_isInvVpDirty = false;
		}

		return m_invViewProj;
	}
}