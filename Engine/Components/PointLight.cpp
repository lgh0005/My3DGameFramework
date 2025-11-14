#include "EnginePch.h"
#include "PointLight.h"
#include "Components/Transform.h"

PointLightUPtr PointLight::Create()
{
	auto pointLight = PointLightUPtr(new PointLight());
	return pointLight;
}