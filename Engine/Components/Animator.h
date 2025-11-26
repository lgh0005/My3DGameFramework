#pragma once
#include "Component.h"
#include "Graphics/Animation.h"

CLASS_PTR(Animator)
class Animator : public Component
{
public:
	static AnimatorUPtr Create(AnimationPtr animation);
	static const ComponentType s_ComponentType = ComponentType::Animator;
	virtual ComponentType GetType() const override { return ComponentType::Animator; }

	void UpdateAnimation();
	void PlayAnimation(AnimationPtr animation);
	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	std::vector<glm::mat4> GetFinalBoneMatrices() { return m_finalBoneMatrices; }

private:
	Animator() = default;
	bool Init(AnimationPtr animation);
	std::vector<glm::mat4> m_finalBoneMatrices;
	AnimationPtr m_currentAnimation;
	float m_currentTime;
};