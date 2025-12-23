#pragma once
#include "Component.h"
#include "Resources/Animation.h"
#include "Graphics/Geometry.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Model)
#pragma endregion

CLASS_PTR(Animator)
class Animator : public Component
{
public:
	static AnimatorUPtr Create(ModelPtr model);
	static const ComponentType s_ComponentType = ComponentType::Animator;
	virtual ComponentType GetComponentType() const override { return ComponentType::Animator; }
	void PlayAnimation(AnimationPtr animation);
	void Update();

	std::vector<glm::mat4> GetFinalBoneMatrices() const { return m_finalBoneMatrices; }
	const RenderBounds& GetCurrentLocalBounds() const { return m_currentLocalAABB; }

/*====================================//
//   animation update logic methods   //
//====================================*/
private:
	void UpdateAnimationTime();
	void UpdateBoneTransforms();
	void UpdateCurrentPoseLocalBounds();

private:
	Animator() = default;
	bool Init(ModelPtr model);

	std::vector<glm::mat4> m_finalBoneMatrices;
	std::vector<glm::mat4> m_globalTransforms; // TEMP
	std::vector<glm::vec3> m_globalJointPositions;

	AnimationPtr m_currentAnimation;
	ModelPtr m_currentModel;

	float m_currentTime;
	RenderBounds m_currentLocalAABB;
};