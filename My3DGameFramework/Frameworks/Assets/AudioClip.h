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
	virtual AudioClipDesc& GetDesc() override { return m_desc; }
	virtual const ResourceDesc& GetDesc() const override { return m_desc; }
	AudioType GetType() const { return m_desc.type; }
	const std::vector<uint8>& GetBuffer() const { return m_audioBuffer; }

private:
	AudioClip();
	bool LoadIntoMemory();
	bool CheckFileExist();

	AudioClipDesc m_desc;
	std::vector<uint8> m_audioBuffer;
};