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
	DEFINE_COMPONENT_TYPE(ComponentType::Animator)

private:
	using AnimBinding = struct AnimationBinding
	{
		uint32 nodeNameHash;       // 해싱된 노드 이름 (Controller 전달용)
		Transform* transform;      // 씬의 실제 Transform 컴포넌트
		Pose defaultPose;          // 미리 Decompose 해둔 초기 포즈 (T-Pose)
	};

public:
	virtual ~Animator();
	static AnimatorUPtr Create(ModelPtr model, AnimControllerUPtr controller);
	virtual void OnStart() override;
	virtual void OnUpdate() override;

	AnimController* GetController() const { return m_controller.get(); }
	const std::vector<glm::mat4>& GetFinalBoneMatrices() const { return m_finalBoneMatrices; }
	const RenderBounds& GetCurrentLocalBounds() const { return m_currentLocalAABB; }

/*==================================//
//   animation initialize methods   //
//==================================*/
private:
	void BindBoneTransforms();
	void BindBoneTransformsFlat(Transform* rootTransform);

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
	std::unordered_map<uint32, Transform*> m_boneTransformMap;
	std::vector<Transform*> m_skinningTransforms;
	std::vector<AnimBinding> m_animationBindings;

	RenderBounds m_currentLocalAABB		{ RenderBounds::Empty() };
};
