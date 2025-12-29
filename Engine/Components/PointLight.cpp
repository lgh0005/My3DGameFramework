#include "EnginePch.h"
#include "PointLight.h"
#include "Components/Transform.h"

PointLight::PointLight() = default;
PointLight::~PointLight() = default;

PointLightUPtr PointLight::Create()
{
	auto pointLight = PointLightUPtr(new PointLight());
	return pointLight;
}