#include "CorePch.h"
#include "TransformUtils.h"

namespace MGF3D
{
	mat4 TransformUtils::ComputeLocalMatrix(const Position& pos, const Rotation& rot, const Scale& scale)
	{
		mat4 T = GetTranslateMatrix(pos.AsVec3());
		mat4 R = GetRotateMatrix(rot.value);
		mat4 S = GetScaleMatrix(scale.AsVec3());
		return T * R * S;
	}

	mat4 TransformUtils::CombineMatrices(const mat4& parentWorld, const mat4& local)
	{
		return parentWorld * local;
	}

	vec3 TransformUtils::GetTranslation(const mat4& matrix)
	{
		return vec3(matrix[3][0], matrix[3][1], matrix[3][2]);
	}

	quat TransformUtils::GetRotation(const mat4& matrix)
	{
		float tr = matrix[0][0] + matrix[1][1] + matrix[2][2];
		quat q;

		if (tr > 0.0f)
		{
			float s = Math::Sqrt(tr + 1.0f) * 2.0f;
			q.w = 0.25f * s;
			q.x = (matrix[1][2] - matrix[2][1]) / s;
			q.y = (matrix[2][0] - matrix[0][2]) / s;
			q.z = (matrix[0][1] - matrix[1][0]) / s;
		}
		else if ((matrix[0][0] > matrix[1][1]) && (matrix[0][0] > matrix[2][2]))
		{
			float s = Math::Sqrt(1.0f + matrix[0][0] - matrix[1][1] - matrix[2][2]) * 2.0f;
			q.w = (matrix[1][2] - matrix[2][1]) / s;
			q.x = 0.25f * s;
			q.y = (matrix[0][1] + matrix[1][0]) / s;
			q.z = (matrix[0][2] + matrix[2][0]) / s;
		}
		else if (matrix[1][1] > matrix[2][2])
		{
			float s = Math::Sqrt(1.0f + matrix[1][1] - matrix[0][0] - matrix[2][2]) * 2.0f;
			q.w = (matrix[2][0] - matrix[0][2]) / s;
			q.x = (matrix[0][1] + matrix[1][1]) / s;
			q.y = 0.25f * s;
			q.z = (matrix[1][2] + matrix[2][1]) / s;
		}
		else
		{
			float s = Math::Sqrt(1.0f + matrix[2][2] - matrix[0][0] - matrix[1][1]) * 2.0f;
			q.w = (matrix[0][1] - matrix[1][0]) / s;
			q.x = (matrix[0][2] + matrix[2][0]) / s;
			q.y = (matrix[1][2] + matrix[2][1]) / s;
			q.z = 0.25f * s;
		}

		return Math::Normalize(q);
	}

	vec3 TransformUtils::GetScale(const mat4& matrix)
	{
		// 각 열(기저 벡터)의 길이를 측정하여 스케일 추출
		vec3 x(matrix[0][0], matrix[0][1], matrix[0][2]);
		vec3 y(matrix[1][0], matrix[1][1], matrix[1][2]);
		vec3 z(matrix[2][0], matrix[2][1], matrix[2][2]);
		return vec3(Math::Length(x), Math::Length(y), Math::Length(z));
	}

	/*===================================//
	//         Basis Vectors             //
	//===================================*/
	vec3 TransformUtils::GetForward(const mat4& matrix)
	{
		// Column 2 (Z-Axis)
		return Math::Normalize(vec3(matrix[2][0], matrix[2][1], matrix[2][2]));
	}

	vec3 TransformUtils::GetUp(const mat4& matrix)
	{
		// Column 1 (Y-Axis)
		return Math::Normalize(vec3(matrix[1][0], matrix[1][1], matrix[1][2]));
	}

	vec3 TransformUtils::GetRight(const mat4& matrix)
	{
		// Column 0 (X-Axis)
		return Math::Normalize(vec3(matrix[0][0], matrix[0][1], matrix[0][2]));
	}

	/*===================================//
	//        Matrix Generation          //
	//===================================*/
	mat4 TransformUtils::GetTranslateMatrix(const vec3& position)
	{
		mat4 result = mat4(1.0f);

		// 4번째 열(Index 3)에 위치 데이터 저장
		result[3][0] = position.x;
		result[3][1] = position.y;
		result[3][2] = position.z;
		result[3][3] = 1.0f;

		return result;
	}

	mat4 TransformUtils::GetRotateMatrix(const quat& rotation)
	{
		mat4 result = mat4(1.0f);

		// 쿼터니언 성분 미리 계산 (수치 안정성 및 속도)
		float xx = rotation.x * rotation.x;
		float yy = rotation.y * rotation.y;
		float zz = rotation.z * rotation.z;
		float xy = rotation.x * rotation.y;
		float xz = rotation.x * rotation.z;
		float yz = rotation.y * rotation.z;
		float wx = rotation.w * rotation.x;
		float wy = rotation.w * rotation.y;
		float wz = rotation.w * rotation.z;

		// Column 0
		result[0][0] = 1.0f - 2.0f * (yy + zz);
		result[0][1] = 2.0f * (xy + wz);
		result[0][2] = 2.0f * (xz - wy);
		result[0][3] = 0.0f;

		// Column 1
		result[1][0] = 2.0f * (xy - wz);
		result[1][1] = 1.0f - 2.0f * (xx + zz);
		result[1][2] = 2.0f * (yz + wx);
		result[1][3] = 0.0f;

		// Column 2
		result[2][0] = 2.0f * (xz + wy);
		result[2][1] = 2.0f * (yz - wx);
		result[2][2] = 1.0f - 2.0f * (xx + yy);
		result[2][3] = 0.0f;

		// Column 3
		result[3][0] = 0.0f;
		result[3][1] = 0.0f;
		result[3][2] = 0.0f;
		result[3][3] = 1.0f;

		return result;
	}

	mat4 TransformUtils::GetScaleMatrix(const vec3& scale)
	{
		mat4 result = mat4(1.0f);

		// 주대각선 성분에 스케일 값 적용
		result[0][0] = scale.x;
		result[1][1] = scale.y;
		result[2][2] = scale.z;
		result[3][3] = 1.0f;

		return result;
	}
}