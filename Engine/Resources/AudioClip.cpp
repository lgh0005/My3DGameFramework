#include "EnginePch.h"
#include "AudioClip.h"

DECLARE_DEFAULTS_IMPL(AudioClip)

AudioClipPtr AudioClip::Load(const AudioClipDesc& desc)
{
	// 1. 리소스 생성 및 기본 설정
	AudioClipPtr audio(new AudioClip());
	audio->m_desc = desc;

	// 2. 타입에 따른 로딩 방식 분기
	switch (desc.type)
	{
		case AudioType::SFX:
		{
			if (!audio->LoadIntoMemory())
			{
				LOG_ERROR("AudioClip::Load - Failed to load SFX: {}", audio->m_desc.path);
				return nullptr;
			}
			break;
		}

		case AudioType::BGM:
		{
			if (!audio->CheckFileExist())
			{
				LOG_ERROR("AudioClip::Load - BGM file not found: {}", audio->m_desc.path);
				return nullptr;
			}
			break;
		}
	}

	return audio;
}

bool AudioClip::LoadIntoMemory()
{
	// 1. 파일을 바이너리 모드, 커서를 맨 끝에 둔 상태로 열기
	std::ifstream file(m_desc.path, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		return false;

	// 2. 파일 크기 측정
	std::streamsize size = file.tellg();
	if (size <= 0) return false;

	// 3. 커서 초기화
	file.seekg(0, std::ios::beg);

	// 4. 버퍼 할당 및 읽기
	m_audioBuffer.resize(size);
	if (!file.read((char*)m_audioBuffer.data(), size))
		return false;

	return true;
}

bool AudioClip::CheckFileExist()
{
	return std::filesystem::exists(m_desc.path);
}
