#pragma once
#include "Object/Component.h"
#include "Resources/Animation.h"
#include "Graphics/Geometry.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Model)
CLASS_PTR(AnimController)
CLASS_PTR(Transform)
#pragma endregion

CLASS_PTR(Animator)
class Animator : public Component
{
public:
	virtual ~Animator();
	static AnimatorUPtr Create(ModelPtr model, AnimControllerUPtr controller);
	static const ComponentType s_ComponentType = ComponentType::Animator;
	virtual ComponentType GetComponentType() const override { return ComponentType::Animator; }
	void Update();

	AnimController* GetController() const { return m_controller.get(); }
	const std::vector<glm::mat4>& GetFinalBoneMatrices() const { return m_finalBoneMatrices; }
	const RenderBounds& GetCurrentLocalBounds() const { return m_currentLocalAABB; }

/*==================================//
//   animation initialize methods   //
//==================================*/
private:
	void BindBoneTransforms();
	void RecursiveBindBoneTransforms(Transform* nodeTransform);

/*==============================//
//   animation update methods   //
//==============================*/
private:
	void UpdateAnimationToTransforms();
	void CalculateFinalBoneMatrices();
	void UpdateCurrentPoseLocalBounds();

private:
	Animator();
	bool Init(ModelPtr model, AnimControllerUPtr controller);

	ModelPtr m_currentModel;
	AnimControllerUPtr m_controller;

	std::vector<glm::mat4> m_finalBoneMatrices;
	std::unordered_map<std::string, Transform*> m_boneTransformMap;
	std::vector<Transform*> m_skinningTransforms;

	RenderBounds m_currentLocalAABB		{ RenderBounds::Empty() };
};
