#pragma once
#include "Components/MeshRenderers/MeshRenderer.h"

namespace MGF3D
{
	MGF_CLASS_PTR(SkinnedMeshRenderer)
	class SkinnedMeshRenderer : public MeshRenderer
	{
		MGF_DISABLE_COPY(SkinnedMeshRenderer)
		using Super = MeshRenderer;

	public:
		SkinnedMeshRenderer
		(
			ObjectIDHash id, ObjectIDHash ownerID,
			SkinnedMeshPtr mesh, StringHash materialHash,
			Animator* animator = nullptr
		);

	/*=========================================//
	//       SkinnedMeshRenderer Type          //
	//=========================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	/*======================================//
	//   SkinnedMeshRenderer methods        //
	//======================================*/
	public:
		void SetAnimator(Animator* animator) { m_animator = animator; }
		Animator* GetAnimator() const { return m_animator; }
		const Vector<mat4>* GetBoneMatrices() const;

	private:
		Animator* m_animator{ nullptr };
	};
}

//#pragma region FORWARD_DECLARATION
//CLASS_PTR(SkinnedMesh)
//CLASS_PTR(Material)
//CLASS_PTR(Animator)
//CLASS_PTR(Program)
//#pragma endregion
//
//CLASS_PTR(SkinnedMeshRenderer)
//class SkinnedMeshRenderer : public MeshRenderer
//{
//	using Super = MeshRenderer;
//	DEFINE_COMPONENT_TYPE(ComponentType::SkinnedMeshRenderer)
//
//public:
//	virtual ~SkinnedMeshRenderer();
//	static SkinnedMeshRendererUPtr Create
//	(
//		SkinnedMeshPtr mesh, 
//		MaterialPtr material, 
//		Animator* animator
//	);
//	virtual RenderBounds GetWorldBounds() const override;
//	Animator* GetAnimator() const;
//	virtual void Render(Program* program) const override;
//
//private:
//	SkinnedMeshRenderer();
//	bool Init(SkinnedMeshPtr mesh, MaterialPtr material, Animator* animator);
//	mutable Animator* m_referenceAnimator{ nullptr };
//};