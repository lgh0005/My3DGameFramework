#pragma once
#include "Component.h"
#include "Resources/Animation.h"
#include "Graphics/Geometry.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Model)
CLASS_PTR(AnimController)
#pragma endregion

CLASS_PTR(Animator)
class Animator : public Component
{
public:
	static AnimatorUPtr Create(ModelPtr model, AnimControllerUPtr controller);
	static const ComponentType s_ComponentType = ComponentType::Animator;
	virtual ComponentType GetComponentType() const override { return ComponentType::Animator; }
	void Update();

	AnimController* GetController() const { return m_controller.get(); }
	std::vector<glm::mat4> GetFinalBoneMatrices() const { return m_finalBoneMatrices; }
	const RenderBounds& GetCurrentLocalBounds() const { return m_currentLocalAABB; }

/*====================================//
//   animation update logic methods   //
//====================================*/
private:
	void UpdateAnimationController();
	void UpdateBoneTransforms();
	void UpdateCurrentPoseLocalBounds();

private:
	Animator() = default;
	bool Init(ModelPtr model, AnimControllerUPtr controller);

	std::vector<glm::mat4> m_finalBoneMatrices;
	std::vector<glm::mat4> m_globalTransforms; // TEMP
	std::vector<glm::vec3> m_globalJointPositions;
	std::vector<Pose>	   m_poses;

	ModelPtr m_currentModel;
	AnimControllerUPtr m_controller;

	RenderBounds m_currentLocalAABB;
};