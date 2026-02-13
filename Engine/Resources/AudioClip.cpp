#include "EnginePch.h"
#include "AudioClip.h"

DECLARE_DEFAULTS_IMPL(AudioClip)

AudioClipPtr AudioClip::Load(const AudioClipDesc& desc)
{
	// 1. 리소스 생성 및 기본 설정
	AudioClipPtr audio(new AudioClip());
	audio->SetPath(desc.path);
	audio->SetName(desc.name);
	audio->m_type = desc.type;

	// 2. 타입에 따른 로딩 방식 분기
	switch (desc.type)
	{
		case AudioType::SFX:
		{
			if (!audio->LoadIntoMemory(desc.path))
			{
				LOG_ERROR("AudioClip::Load - Failed to load SFX: {}", desc.path);
				return nullptr;
			}
			break;
		}

		case AudioType::BGM:
		{
			if (!audio->CheckFileExist(desc.path))
			{
				LOG_ERROR("AudioClip::Load - BGM file not found: {}", desc.path);
				return nullptr;
			}
			break;
		}
	}

	return audio;
}

bool AudioClip::LoadIntoMemory(const std::string& filepath)
{
	// 1. 파일을 바이너리 모드, 커서를 맨 끝에 둔 상태로 열기
	std::ifstream file(filepath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		return false;

	// 2. 파일 크기 측정
	std::streamsize size = file.tellg();
	if (size <= 0)
		return false;

	// 3. 커서 초기화
	file.seekg(0, std::ios::beg);

	// 4. 버퍼 할당 및 읽기
	m_audioBuffer.resize(size);
	if (!file.read((char*)m_audioBuffer.data(), size))
		return false;

	return true;
}

bool AudioClip::CheckFileExist(const std::string& filepath)
{
	std::ifstream file(filepath);
	return file.good();
}
