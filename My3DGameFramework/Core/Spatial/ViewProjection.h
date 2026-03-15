#pragma once

namespace MGF3D
{
	class ViewProjection
	{
	public:
		ViewProjection();

		void UpdateView(const mat4& cameraWorldMatrix);
		void SetPerspective(float fovDeg, float aspect, float nearZ, float farZ);

		const mat4& GetView() const { return m_view; }
		const mat4& GetProj() const { return m_proj; }
		const mat4& GetVP()   const;
		const mat4& GetInvVP() const;

	private:
		mat4 m_view{ 1.0f };
		mat4 m_proj{ 1.0f };

		mutable mat4 m_viewProj{ 1.0f };
		mutable bool m_isVpDirty{ true };

		mutable mat4 m_invViewProj{ 1.0f };
		mutable bool m_isInvVpDirty{ true };
	};
}