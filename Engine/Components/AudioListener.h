#pragma once
#include "Object/Component.h"

CLASS_PTR(AudioListener)
class AudioListener : public Component
{
public:
	virtual ~AudioListener();
	static AudioListenerUPtr Create();
	static const ComponentType s_ComponentType = ComponentType::AudioListener;
	virtual ComponentType GetComponentType() const override { return ComponentType::AudioListener; }
	virtual void OnUpdate() override;

private:
	AudioListener();;
};