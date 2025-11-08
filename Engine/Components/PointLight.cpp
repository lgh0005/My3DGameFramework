#include "EnginePch.h"
#include "PointLight.h"
#include "Components/Transform.h"

PointLightUPtr PointLight::Create()
{
	auto pointLight = PointLightUPtr(new PointLight());
	if (!pointLight->Init()) return nullptr;
	return pointLight;
}

bool PointLight::Init()
{
	m_transform = Transform::Create();
	if (!m_transform) return false;
	return true;
}