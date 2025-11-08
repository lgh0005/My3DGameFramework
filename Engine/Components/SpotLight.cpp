#include "EnginePch.h"
#include "SpotLight.h"
#include "Components/Transform.h"

SpotLightUPtr SpotLight::Create()
{
	auto spotLight = SpotLightUPtr(new SpotLight());
	if (!spotLight->Init()) return nullptr;
	return spotLight;
}

bool SpotLight::Init()
{
	m_transform = Transform::Create();
	if (!m_transform) return false;
	return true;
}
