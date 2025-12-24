#include "EnginePch.h"
#include "AnimState.h"
#include "Resources/Animation.h"

AnimState::AnimState() = default;

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
	m_transitions[targetStateName] = duration;
}

float AnimState::GetTransitionDuration(const std::string& targetStateName) const
{
	auto it = m_transitions.find(targetStateName);
	if (it != m_transitions.end()) return it->second;
	return -1.0f;
}
