#pragma once
#include "Entities/Component.h"
#include "Geometry/RenderBounds.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Model)
	MGF_CLASS_PTR(AnimController)

	MGF_CLASS_PTR(Animator)
	class Animator : public Component
	{
		MGF_DISABLE_COPY(Animator)
		using Super = Component;

	public:
		Animator
		(
			ObjectIDHash id, ObjectIDHash ownerID,
			const ModelPtr& model, AnimControllerUPtr controller
		);
		virtual ~Animator() override;
		Animator(Animator&& other) noexcept;
		Animator& operator=(Animator&& other) noexcept;

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	/*================================//
	//        Animator Interface      //
	//================================*/
	public:
		void Update(float deltaTime);
		AnimController* GetController() const { return m_controller.get(); }
		const Vector<mat4>& GetFinalBoneMatrices() const { return m_finalBoneMatrices; }
		const RenderBounds& GetCurrentLocalBounds() const { return m_currentLocalAABB; }
		void SetBoneBufferOffset(uint32 offset) { m_boneBufferOffset = offset; }
		uint32 GetBoneBufferOffset() const { return m_boneBufferOffset; }

	private:
		void CalculateBoneMatrices();
		void UpdateLocalBounds();

		ModelPtr m_currentModel;
		AnimControllerUPtr m_controller;

		uint32 m_boneBufferOffset{ 0 };
		Vector<mat4> m_globalBoneMatrices;
		Vector<mat4> m_finalBoneMatrices;

		RenderBounds m_currentLocalAABB{ RenderBounds::Empty() };
	};
}

