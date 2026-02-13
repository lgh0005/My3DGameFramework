#pragma once
#include "Resources/Resource.h"
#include "Resources/ResourceDesc.h"

CLASS_PTR(AudioClip)
class AudioClip : public Resource
{
	DEFINE_RESOURCE_TYPE(ResourceType::AudioClip, AudioClipDesc)

public:
	virtual ~AudioClip();
	static AudioClipPtr Load(const AudioClipDesc& desc);
	AudioType GetType() const { return m_type; }
	const std::vector<uint8>& GetBuffer() const { return m_audioBuffer; }

private:
	AudioClip();
	bool LoadIntoMemory(const std::string& filepath);
	bool CheckFileExist(const std::string& filepath);

	AudioType		   m_type;
	std::vector<uint8> m_audioBuffer;
};