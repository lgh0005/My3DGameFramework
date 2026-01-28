#pragma once
#include "Resources/Resource.h"

CLASS_PTR(AudioClip)
class AudioClip : public Resource
{
	DEFINE_RESOURCE_TYPE(ResourceType::AudioClip)

public:
	virtual ~AudioClip();
	static AudioClipUPtr LoadSFX(const std::string& filepath);
	static AudioClipUPtr LoadBGM(const std::string& filepath);

	AudioType GetType() const { return m_type; }
	const std::vector<uint8>& GetBuffer() const { return m_audioBuffer; }
	const std::string& GetPath() const { return m_path; }

private:
	AudioClip();
	bool InitSFX(const std::string& filepath);
	bool InitBGM(const std::string& filepath);
	
	AudioType		   m_type;
	std::string		   m_path;
	std::vector<uint8> m_audioBuffer;
};