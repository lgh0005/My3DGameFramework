#include "EnginePch.h"
#include "SpotLight.h"
#include "Components/Transform.h"

SpotLightUPtr SpotLight::Create()
{
	auto spotLight = SpotLightUPtr(new SpotLight());
	return spotLight;
}
