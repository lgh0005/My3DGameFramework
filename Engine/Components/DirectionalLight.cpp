#include "EnginePch.h"
#include "DirectionalLight.h"
#include "Components/Transform.h"

DirectionalLightUPtr DirectionalLight::Create()
{
	auto pointLight = DirectionalLightUPtr(new DirectionalLight());
	if (!pointLight) return nullptr;
	return pointLight;
}
