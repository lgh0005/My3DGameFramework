#pragma once
#include "Entities/Mechanic.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Transform)

	MGF_CLASS_PTR(TransformCoordinate)
	class TransformCoordinate : public Mechanic
	{
		MGF_DISABLE_COPY(TransformCoordinate)
		using Super = Mechanic;

	public:
		TransformCoordinate(Component* owner);
		virtual ~TransformCoordinate() override;
		TransformCoordinate(TransformCoordinate&& other) noexcept;
		TransformCoordinate& operator=(TransformCoordinate&& other) noexcept;

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	/*===================================//
	//      Direction Vectors            //
	//===================================*/
	public:
		vec3 CalculateForward(const quat& rotation) const;
		vec3 CalculateRight(const quat& rotation) const;
		vec3 CalculateUp(const quat& rotation) const;

	/*===================================//
	//      Space Transformations        //
	//===================================*/
	public:
		// 로컬 좌표를 월드 좌표로 변환
		vec3 TransformPoint(const vec3& localPoint, const mat4& worldMatrix) const;

		// 월드 좌표를 로컬 좌표로 변환
		vec3 InverseTransformPoint(const vec3& worldPoint, const mat4& worldMatrix) const;

	private:
		Transform* GetTransformOwner() const;
	};
}