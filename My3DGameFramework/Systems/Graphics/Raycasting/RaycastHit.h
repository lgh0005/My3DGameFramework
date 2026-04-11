#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(GameObject)

	MGF_CLASS_PTR(RaycasterHit)
	class RaycastHit
	{
	public:
		RaycastHit();
		~RaycastHit();

		void Reset();
		void SetHitResult
		(
			GameObject* obj,
			const vec3& point,
			const vec3& normal,
			float dist
		);

		bool		HasHit()		const { return m_isHit; }
		GameObject* GetGameObject() const { return m_gameObject; }
		const vec3& GetPoint()		const { return m_point; }
		const vec3& GetNormal()		const { return m_normal; }
		float		GetDistance()	const { return m_distance; }

	private:
		GameObject* m_gameObject { nullptr };		// 충돌한 오브젝트
		vec3	    m_point		 { vec3(0.f) };		// 월드 충돌 좌표
		vec3	    m_normal	 { vec3(0.f) };		// 월드 표면 법선
		float		m_distance	 { 0.f };			// 거리
		bool		m_isHit		 { false };			// 충돌 성공 여부
	};
}