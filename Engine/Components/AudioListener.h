#pragma once
#include "Components/Component.h"

CLASS_PTR(AudioListener)
class AudioListener : public Component
{
public:
	virtual ~AudioListener() = default;
	static AudioListenerUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::AudioListener;
	virtual ComponentType GetType() const override { return ComponentType::AudioListener; }
	void Update();

private:
	AudioListener() = default;
};