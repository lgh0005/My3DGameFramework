#pragma once
#include "Graphics/Animation.h"

CLASS_PTR(Animator)
class Animator
{
public:
	static AnimatorUPtr Create(AnimationUPtr animation);

	void UpdateAnimation();
	void PlayAnimation(AnimationUPtr animation);
	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	std::vector<glm::mat4> GetFinalBoneMatrices() { return m_finalBoneMatrices; }

private:
	Animator() = default;
	bool Init(AnimationUPtr animation);
	std::vector<glm::mat4> m_finalBoneMatrices;
	AnimationUPtr m_currentAnimation;
	float m_currentTime;
};