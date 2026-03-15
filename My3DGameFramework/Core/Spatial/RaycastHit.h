#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(IRaycastTarget)

	struct RaycastHit
	{
		Ptr<IRaycastTarget> target	 { nullptr };
		vec3			    point	 { 0.0f };
		vec3				normal	 { 0.0f };
		float				distance { 0.0f };
		bool				isHit	 { false };

		RaycastHit();
		RaycastHit
		(
			Ptr<IRaycastTarget> t,
			const vec3& p,
			const vec3& n,
			float d
		);

		void Reset();
		void Set
		(
			Ptr<IRaycastTarget> t,
			const vec3& pos,
			const vec3& norm, float dist
		);
	};
}

