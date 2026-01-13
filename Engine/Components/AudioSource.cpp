#include "EnginePch.h"
#include "AudioSource.h"
#include "Components/Transform.h"

// TODO : SFX랑 BGM을 분기해서 처리하는 것을 조금 명확히 코드로 작성할
// 필요가 있음
AudioSource::AudioSource() = default;
AudioSource::~AudioSource()
{
	Reset();
}

AudioSourceUPtr AudioSource::Create(const AudioClipPtr& audioClip)
{
	auto audioSource = AudioSourceUPtr(new AudioSource());
	if (!audioSource->Init(audioClip)) return nullptr;
	return std::move(audioSource);
}

bool AudioSource::Init(const AudioClipPtr& audioClip)
{
	Reset();
	if (audioClip == nullptr) return false;

	ma_engine* engine = AUDIO.GetEngine();
	AudioType type = audioClip->GetType();
	ma_sound_group* group = AUDIO.GetGroup(type);
	bool success = false;

	switch (type)
	{
		case AudioType::BGM:
		{
			success = CreateBGM(engine, group, audioClip);
			break;
		}

		case AudioType::SFX:
		{
			success = CreateSFX(engine, group, audioClip);
			break;
		}

		default:
			LOG_WARN("Unknown AudioType");
			return false;
	}

	if (!success)
	{
		LOG_ERROR("Failed to init sound.");
		return false;
	}

	m_clip = audioClip;
	Set3DAttributes(1.0f, 30.0f);

	return true;
}

void AudioSource::OnUpdate()
{
	// 로드 안됐거나 재생 중 아니면 굳이 연산 X
	if (m_clip == nullptr) return;
	if (!ma_sound_is_playing(&m_sound)) return;

	// 1. 위치 동기화
	auto transform = GetTransform();
	auto pos = transform.GetWorldPosition();
	ma_sound_set_position(&m_sound, pos.x, pos.y, pos.z);

	// 2. 방향 동기화
	glm::vec3 fwd = transform.GetForwardVector();
	ma_sound_set_direction(&m_sound, fwd.x, fwd.y, fwd.z);
}

void AudioSource::Play()
{
	if (m_clip == nullptr) return;

	// 만약 SFX라면, 쏠 때마다 처음부터 다시 재생하는 게 자연스러움
	if (m_clip->GetType() == AudioType::SFX)
		ma_sound_seek_to_pcm_frame(&m_sound, 0);

	ma_sound_start(&m_sound);
}

void AudioSource::Stop()
{
	if (m_clip == nullptr) return;
	ma_sound_stop(&m_sound);
}

void AudioSource::SetLoop(bool loop)
{
	if (m_clip == nullptr) return;
	ma_sound_set_looping(&m_sound, loop);
}

void AudioSource::SetVolume(float volume)
{
	if (m_clip == nullptr) return;
	ma_sound_set_volume(&m_sound, volume);
}

void AudioSource::Set3DAttributes(float minDist, float maxDist)
{
	if (m_clip == nullptr) return;
	ma_sound_set_min_distance(&m_sound, minDist);
	ma_sound_set_max_distance(&m_sound, maxDist);
}

void AudioSource::Reset()
{
	if (m_clip == nullptr) return;

	// 사운드 해제
	ma_sound_uninit(&m_sound);
	if (m_clip->GetType() == AudioType::SFX)
	{
		ma_decoder_uninit(&m_decoder);
	}

	m_clip = nullptr;
}

bool AudioSource::CreateSFX(ma_engine* engine, ma_sound_group* group, const AudioClipPtr& clip)
{
	const auto& buffer = clip->GetBuffer();
	if (buffer.empty()) return false;

	// 1. 디코더 초기화
	ma_decoder_config config = ma_decoder_config_init_default();
	ma_result result = ma_decoder_init_memory(buffer.data(), buffer.size(), &config, &m_decoder);

	if (result != MA_SUCCESS) return false;

	// 2. 사운드 생성 (Data Source)
	result = ma_sound_init_from_data_source(engine, &m_decoder, 0, group, &m_sound);

	if (result != MA_SUCCESS)
	{
		// 사운드 생성 실패 시, 앞에서 만든 디코더도 해제해야 함
		ma_decoder_uninit(&m_decoder);
		return false;
	}

	// 3. SFX 설정 (3D 켜기 + 거리 설정)
	ma_sound_set_spatialization_enabled(&m_sound, MA_TRUE);
	Set3DAttributes(1.0f, 30.0f); // 기본 거리 설정

	return true;
}

bool AudioSource::CreateBGM(ma_engine* engine, ma_sound_group* group, const AudioClipPtr& clip)
{
	// 1. 사운드 생성 (File Stream)
	ma_result result = ma_sound_init_from_file
	(
		engine,
		clip->GetPath().c_str(),
		MA_SOUND_FLAG_STREAM,
		group,
		NULL,
		&m_sound
	);

	if (result != MA_SUCCESS) return false;

	// 2. BGM 설정 (2D + Loop)
	ma_sound_set_spatialization_enabled(&m_sound, MA_TRUE);
	ma_sound_set_looping(&m_sound, true);

	return true;
}