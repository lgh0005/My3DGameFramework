#include "CorePch.h"
#include "RaycastHit.h"

namespace MGF3D
{
	RaycastHit::RaycastHit() = default;
	RaycastHit::RaycastHit
	(
		Ptr<IRaycastTarget> t,
		const vec3& p, 
		const vec3& n, 
		float d
	) : target(t), point(p), normal(n), distance(d), isHit(true) { }

	void RaycastHit::Set(Ptr<IRaycastTarget> t, const vec3& p, const vec3& n, float d)
	{
		target = t;
		point = p;
		normal = n;
		distance = d;
		isHit = true;
	}

	void RaycastHit::Reset()
	{
		*this = RaycastHit();
	}
}