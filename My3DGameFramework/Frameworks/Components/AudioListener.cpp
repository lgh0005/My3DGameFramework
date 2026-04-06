#include "EnginePch.h"
#include "AudioListener.h"
#include "Components/Transform.h"

DECLARE_DEFAULTS_IMPL(AudioListener)

AudioListenerUPtr AudioListener::Create()
{
	return AudioListenerUPtr(new AudioListener());
}

void AudioListener::OnUpdate()
{
	// 1. 트랜스폼 가져오기
	auto transform = GetTransform();

	// 2. 위치 및 방향 데이터 추출
	// TODO: 계층 구조 도입 시 GetWorldPosition() 사용
	glm::vec3 pos = transform.GetWorldPosition();
	glm::vec3 fwd = transform.GetForwardVector();
	glm::vec3 up = transform.GetUpVector();

	// 3. 엔진에게 전달
	ma_engine* engine = AUDIO.GetEngine();
	ma_engine_listener_set_position(engine, 0, pos.x, pos.y, pos.z);
	ma_engine_listener_set_direction(engine, 0, fwd.x, fwd.y, fwd.z);
	ma_engine_listener_set_world_up(engine, 0, up.x, up.y, up.z);

	// 4. TODO : 도플러 효과
	// (선택) 도플러 효과를 위해 속도(Velocity)도 넣어줄 수 있지만, 
	// 지금은 물리 엔진이나 속도 계산 로직이 없으니 패스합니다.
}