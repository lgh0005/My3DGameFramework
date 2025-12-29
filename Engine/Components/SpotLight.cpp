#include "EnginePch.h"
#include "SpotLight.h"
#include "Components/Transform.h"

SpotLight::SpotLight() = default;
SpotLight::~SpotLight() = default;

SpotLightUPtr SpotLight::Create()
{
	auto spotLight = SpotLightUPtr(new SpotLight());
	return spotLight;
}
