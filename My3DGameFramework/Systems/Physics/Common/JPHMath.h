#pragma once
#include <Jolt/Jolt.h>

namespace MGF3D
{
	class JPHMath
	{
		MGF_DECLARE_UTILITIES(JPHMath)

	public:

		/*=============//
		//   Vectors   //
		//=============*/
		static JPH::Vec3 ToJPH(const vec3& v);
		static vec3 FromJPH(const JPH::Vec3& v);
		static JPH::Vec4 ToJPH(const vec4& v);
		static vec4 FromJPH(const JPH::Vec4& v);

		/*===============//
		//   Quaternion  //
		//===============*/
		static JPH::Quat ToJPH(const quat& q);
		static quat FromJPH(const JPH::Quat& q);

		/*============//
		//   Matrix   //
		//============*/
		static JPH::Mat44 ToJPH(const mat4& m);
		static mat4 FromJPH(const JPH::Mat44& m);
	};
}

#include "Utils/JPHMath.inl"