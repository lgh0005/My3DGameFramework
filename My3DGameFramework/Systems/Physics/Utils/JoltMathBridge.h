#pragma once
#include <Jolt/Math/Vec3.h>
#include <Jolt/Math/Vec4.h>
#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Mat44.h>

namespace MGF3D
{
	class JoltMathBridge
	{
		MGF_DECLARE_UTILITIES(JoltMathBridge)

	/*=============================//
	//        glm to Jolt          //
	//=============================*/
	public:
		static JPH::Vec3 ToJoltVec3(const vec3& v);
		static JPH::Vec4 ToJoltVec4(const vec4& v);
		static JPH::Quat ToJoltQuat(const quat& q);
		static JPH::Mat44 ToJoltMat44(const mat4& m);

	/*=============================//
	//        Jolt to glm          //
	//=============================*/
	public:
		static vec3 ToGLMVec3(const JPH::Vec3& v);
		static vec4 ToGLMVec4(const JPH::Vec4& v);
		static quat ToGLMQuat(const JPH::Quat& q);
		static mat4 ToGLMMat44(const JPH::Mat44& m);
	};
}

#include "Utils/JoltMathBridge.inl"