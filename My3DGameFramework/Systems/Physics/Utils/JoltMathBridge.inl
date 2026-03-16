#include "JoltMathBridge.h"
#pragma once

namespace MGF3D
{
	/*=============================//
	//        glm to Jolt          //
	//=============================*/
	inline JPH::Vec3 MGF3D::JoltMathBridge::ToJoltVec3(const vec3& v)
	{
		return JPH::Vec3(v.x, v.y, v.z);
	}

	inline JPH::Vec4 JoltMathBridge::ToJoltVec4(const vec4& v)
	{
		return JPH::Vec4(v.x, v.y, v.z, v.w);
	}

	inline JPH::Quat JoltMathBridge::ToJoltQuat(const quat& q)
	{
		return JPH::Quat(q.x, q.y, q.z, q.w);
	}

	inline JPH::Mat44 JoltMathBridge::ToJoltMat44(const mat4& m)
	{
		return ::JPH::Mat44
		(
			ToJoltVec4(Math::GetColumn(m, 0)),
			ToJoltVec4(Math::GetColumn(m, 1)),
			ToJoltVec4(Math::GetColumn(m, 2)),
			ToJoltVec4(Math::GetColumn(m, 3))
		);
	}

	/*=============================//
	//        Jolt to glm          //
	//=============================*/
	inline vec3 JoltMathBridge::ToGLMVec3(const::JPH::Vec3& v)
	{
		return vec3(v.GetX(), v.GetY(), v.GetZ());
	}

	inline vec4 JoltMathBridge::ToGLMVec4(const::JPH::Vec4& v)
	{
		return vec4(v.GetX(), v.GetY(), v.GetZ(), v.GetW());
	}

	inline quat JoltMathBridge::ToGLMQuat(const::JPH::Quat& q)
	{
		return quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());
	}

	inline mat4 JoltMathBridge::ToGLMMat44(const::JPH::Mat44& m)
	{
		return mat4
		(
			ToGLMVec4(m.GetColumn4(0)),
			ToGLMVec4(m.GetColumn4(1)),
			ToGLMVec4(m.GetColumn4(2)),
			ToGLMVec4(m.GetColumn4(3))
		);
	}
}