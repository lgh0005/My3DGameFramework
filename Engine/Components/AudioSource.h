#pragma once
#include "Object/Component.h"
#include "Resources/AudioClip.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(AudioClip)
#pragma endregion

CLASS_PTR(AudioSource)
class AudioSource : public Component
{
	DEFINE_COMPONENT_TYPE(ComponentType::AudioSource)

public:
	virtual ~AudioSource();
	static AudioSourceUPtr Create(const AudioClipPtr& audioClip);
	virtual void OnUpdate() override;

	void Play();
	void Stop();
	void SetLoop(bool loop);
	void SetVolume(float volume);

	void Set3DAttributes(float minDist, float maxDist);

private:
	AudioSource();
	bool Init(const AudioClipPtr& audioClip);
	void Reset();

	bool CreateSFX(ma_engine* engine, ma_sound_group* group, const AudioClipPtr& clip);
	bool CreateBGM(ma_engine* engine, ma_sound_group* group, const AudioClipPtr& clip);

	AudioClipPtr	m_clip		  { nullptr };
	ma_sound		m_sound 	  {  };		  // 사운드 객체
	ma_decoder		m_decoder	  {  };		  // 메모리 디코딩용 (SFX일 때만 사용)
};