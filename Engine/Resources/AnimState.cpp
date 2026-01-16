#include "EnginePch.h"
#include "AnimState.h"
#include "Resources/Animation.h"
#include "Misc/Utils.h"

DECLARE_DEFAULTS_IMPL(AnimState)

AnimStateUPtr AnimState::Create(const std::string& name, AnimationPtr clip)
{
	auto state = AnimStateUPtr(new AnimState());
	state->Init(name, clip);
	return std::move(state);
}

void AnimState::Init(const std::string& name, AnimationPtr clip)
{
	m_name = name;
	m_clip = clip;
}

void AnimState::AddTransition(const std::string& targetStateName, float duration)
{
	uint32 hash = Utils::StrHash(targetStateName);
	m_transitions[hash] = duration;
}

float AnimState::GetTransitionDuration(const std::string& targetStateName) const
{
	uint32 hash = Utils::StrHash(targetStateName);
	auto it = m_transitions.find(hash);
	if (it != m_transitions.end()) return it->second;
	return -1.0f;
}
