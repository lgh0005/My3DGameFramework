#include "EnginePch.h"
#include "PointLight.h"
#include "Components/Transform.h"

DECLARE_DEFAULTS_IMPL(PointLight)

PointLightUPtr PointLight::Create()
{
	auto pointLight = PointLightUPtr(new PointLight());
	return pointLight;
}