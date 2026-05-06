#include "FrameworkPch.h"
#include "AnimState.h"
#include "Assets/Animation.h"

namespace MGF3D
{
	AnimState::AnimState(const String& name, const AnimationPtr& anim)
		: m_name(name), m_animation(anim)
	{
		m_nameHash = StringHash(name);
	}
	AnimState::~AnimState() = default;
	AnimState::AnimState(AnimState&& other) noexcept = default;
	AnimState& AnimState::operator=(AnimState&& other) noexcept = default;

	void AnimState::AddTransition(StringHash targetStateHash, float duration)
	{
		m_transitions[targetStateHash] = duration;
	}

	float AnimState::GetTransitionDuration(StringHash targetStateHash) const
	{
		auto it = m_transitions.find(targetStateHash);
		if (it != m_transitions.end()) return it->second;
		return -1.0f;
	}
}