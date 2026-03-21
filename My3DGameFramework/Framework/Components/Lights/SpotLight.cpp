#include "EnginePch.h"
#include "SpotLight.h"
#include "Components/Transform.h"

DECLARE_DEFAULTS_IMPL(SpotLight)

SpotLightUPtr SpotLight::Create()
{
	auto spotLight = SpotLightUPtr(new SpotLight());
	return spotLight;
}
