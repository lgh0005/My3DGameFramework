#include "JPHMath.h"
#pragma once

namespace MGF3D
{
    /*=============//
    //   Vectors   //
    //=============*/
    inline JPH::Vec3 JPHMath::ToJPH(const vec3& v)
    {
        return JPH::Vec3(v.x, v.y, v.z);
    }

    inline vec3 JPHMath::FromJPH(const JPH::Vec3& v)
    {
        return vec3(v.GetX(), v.GetY(), v.GetZ());
    }

    inline JPH::Vec4 JPHMath::ToJPH(const vec4& v)
    {
        return JPH::Vec4(v.x, v.y, v.z, v.w);
    }

    inline vec4 JPHMath::FromJPH(const JPH::Vec4& v)
    {
        return vec4(v.GetX(), v.GetY(), v.GetZ(), v.GetW());
    }

    /*===============//
    //   Quaternion  //
    //===============*/
    inline JPH::Quat JPHMath::ToJPH(const quat& q)
    {
        return JPH::Quat(q.x, q.y, q.z, q.w);
    }

    inline quat JPHMath::FromJPH(const JPH::Quat& q)
    {
        return quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ());
    }

    /*============//
    //   Matrix   //
    //============*/
    inline JPH::Mat44 JPHMath::ToJPH(const mat4& m)
    {
        return JPH::Mat44
        (
            ToJPH(Math::GetColumn(m, 0)),
            ToJPH(Math::GetColumn(m, 1)),
            ToJPH(Math::GetColumn(m, 2)),
            ToJPH(Math::GetColumn(m, 3))
        );
    }

    inline mat4 JPHMath::FromJPH(const JPH::Mat44& m)
    {
        mat4 result;
        result[0] = FromJPH(m.GetColumn4(0));
        result[1] = FromJPH(m.GetColumn4(1));
        result[2] = FromJPH(m.GetColumn4(2));
        result[3] = FromJPH(m.GetColumn4(3));
        return result;
    }
}