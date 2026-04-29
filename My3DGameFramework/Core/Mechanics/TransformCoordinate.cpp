#include "CorePch.h"
#include "TransformCoordinate.h"
#include "Components/Transform.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	TransformCoordinate::TransformCoordinate(Component* owner) : Super(owner) {}
	TransformCoordinate::~TransformCoordinate() = default;
	TransformCoordinate::TransformCoordinate(TransformCoordinate&& other) noexcept = default;
	TransformCoordinate& TransformCoordinate::operator=(TransformCoordinate&& other) noexcept = default;

	/*===============================//
	//      Transform Type           //
	//===============================*/
	int16 TransformCoordinate::s_typeIndex = -1;
	const MGFType* TransformCoordinate::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Mechanic");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	vec3 TransformCoordinate::CalculateForward(const quat& rotation) const
	{
		return rotation * vec3(0.0f, 0.0f, -1.0f);
	}

	vec3 TransformCoordinate::CalculateRight(const quat& rotation) const
	{
		return rotation * vec3(1.0f, 0.0f, 0.0f);
	}

	vec3 TransformCoordinate::CalculateUp(const quat& rotation) const
	{
		return rotation * vec3(0.0f, 1.0f, 0.0f);
	}

	vec3 TransformCoordinate::TransformPoint(const vec3& localPoint, const mat4& worldMatrix) const
	{
		return vec3(worldMatrix * vec4(localPoint, 1.0f));
	}

	vec3 TransformCoordinate::InverseTransformPoint(const vec3& worldPoint, const mat4& worldMatrix) const
	{
		return vec3(Math::Inverse(worldMatrix) * vec4(worldPoint, 1.0f));
	}

	Transform* TransformCoordinate::GetTransformOwner() const
	{
		return static_cast<Transform*>(GetOwner());
	}
}