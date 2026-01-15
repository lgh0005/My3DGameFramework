#pragma once
#include "Object/Component.h"

CLASS_PTR(AudioListener)
class AudioListener : public Component
{
	DEFINE_COMPONENT_TYPE(ComponentType::AudioListener)

public:
	virtual ~AudioListener();
	static AudioListenerUPtr Create();
	virtual void OnUpdate() override;

private:
	AudioListener();
};