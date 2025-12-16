#pragma once
#include "Component.h"
#include "Resources/Animation.h"
#include "Graphics/Geometry.h"

CLASS_PTR(Animator)
class Animator : public Component
{
public:
	static AnimatorUPtr Create(AnimationPtr animation);
	static const ComponentType s_ComponentType = ComponentType::Animator;
	virtual ComponentType GetComponentType() const override { return ComponentType::Animator; }

	void Update();
	void PlayAnimation(AnimationPtr animation);
	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	std::vector<glm::mat4> GetFinalBoneMatrices() { return m_finalBoneMatrices; }
	const RenderBounds& GetCurrentLocalBounds() const { return m_currentLocalAABB; }

private:
	Animator() = default;
	bool Init(AnimationPtr animation);
	std::vector<glm::mat4> m_finalBoneMatrices;
	std::vector<glm::vec3> m_globalJointPositions;

	AnimationPtr m_currentAnimation;
	float m_currentTime;

	void CalculateCurrentPoseLocalBounds();
	RenderBounds m_currentLocalAABB;
};