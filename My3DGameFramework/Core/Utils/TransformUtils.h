#pragma once
#include "Spatial/Position.h"
#include "Spatial/Rotation.h"
#include "Spatial/Scale.h"

namespace MGF3D
{
	class TransformUtils
	{
		MGF_DECLARE_UTILITIES(TransformUtils)

	public:

		// 로컬 행렬 조립: T * R * S
		static mat4 ComputeLocalMatrix(const Position& pos, const Rotation& rot, const Scale& scale);

		// 월드 행렬 결합: Parent * Local
		static mat4 CombineMatrices(const mat4& parentWorld, const mat4& local);

		// 행렬에서 특정 데이터 추출
		static vec3 GetTranslation(const mat4& matrix);
		static quat GetRotation(const mat4& matrix);
		static vec3 GetScale(const mat4& matrix);

		// 방향 벡터 추출
		static vec3 GetForward(const mat4& matrix);
		static vec3 GetUp(const mat4& matrix);
		static vec3 GetRight(const mat4& matrix);

		// 개별 행렬 생성 유틸리티
		static mat4 GetTranslateMatrix(const vec3& position);
		static mat4 GetRotateMatrix(const quat& rotation);
		static mat4 GetScaleMatrix(const vec3& scale);
	};
}