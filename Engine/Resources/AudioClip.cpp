#include "EnginePch.h"
#include "AudioClip.h"
#include <fstream>

AudioClip::AudioClip() = default;
AudioClip::~AudioClip() = default;

AudioClipUPtr AudioClip::LoadSFX(const std::string& filepath)
{
	auto sfx = AudioClipUPtr(new AudioClip());
	if (!sfx->InitSFX(filepath)) return nullptr;
	return std::move(sfx);
}

AudioClipUPtr AudioClip::LoadBGM(const std::string& filepath)
{
	auto bgm = AudioClipUPtr(new AudioClip());
	if (!bgm->InitBGM(filepath)) return nullptr;
	return std::move(bgm);
}

bool AudioClip::InitSFX(const std::string& filepath)
{
	m_type = AudioType::SFX;
	m_path = filepath;

	// 1. 파일을 바이너리 모드, 커서를 맨 끝에 둔 상태로 열기
	std::ifstream file(filepath, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		SPDLOG_ERROR("Failed to open audio file: {}", filepath);
		return false;
	}

	// 2. 파일 크기 측정
	std::streamsize size = file.tellg();
	if (size <= 0)
	{
		SPDLOG_ERROR("Audio file is empty: {}", filepath);
		return false;
	}

	// 3. 커서를 다시 파일의 맨 처음으로 돌립니다.
	file.seekg(0, std::ios::beg);

	// 4. 버퍼 크기 할당 및 데이터 읽기
	m_audioBuffer.resize(size);
	if (!file.read((char*)m_audioBuffer.data(), size))
	{
		SPDLOG_ERROR("Failed to read audio file: {}", filepath);
		return false;
	}

	return true;
}

bool AudioClip::InitBGM(const std::string& filepath)
{
	m_type = AudioType::BGM;
	m_path = filepath;

	// 파일이 실제로 존재하는지 정도만 체크
	std::ifstream file(filepath);
	if (!file.good())
	{
		SPDLOG_ERROR("BGM file not found: {}", filepath);
		return false;
	}

	return true;
}