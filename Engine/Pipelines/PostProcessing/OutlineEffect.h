#pragma once
#include "Pipelines/PostProcessing/PostProcessEffect.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(OutlineFramebuffer)
#pragma endregion

CLASS_PTR(OutlineEffect)
class OutlineEffect : public PostProcessEffect
{
	using Super = PostProcessEffect;

public:
	virtual ~OutlineEffect() override;
	static OutlineEffectUPtr Create
	(
		int32 priority,
		int32 width = WINDOW_WIDTH,
		int32 height = WINDOW_HEIGHT,
		const glm::vec3& color = { 1.0f, 1.0f, 0.5f },
		float thickness = 2.0f
	);
	virtual bool Render
	(
		RenderContext* context,
		Framebuffer* mainFBO,
		ScreenMesh* screenMesh
	) override;
	virtual void OnResize(int32 width, int32 height) override;

	/*==============================//
	//   outlining helper methods   //
	//==============================*/
private:
	void MaskMeshes(const std::vector<MeshOutline*>& outlines);
	void MaskStaticMeshes(const std::vector<MeshOutline*>& outlines);
	void MaskSkinnedMeshes(const std::vector<MeshOutline*>& outlines);

private:
	OutlineEffect();
	bool Init
	(
		int32 priority, int32 width, int32 height,
		const glm::vec3& color, float thickness
	);

	OutlineFramebufferUPtr m_maskFBO;
	ProgramPtr m_maskStaticProgram;  // Static 마스크용
	ProgramPtr m_maskSkinnedProgram; // Skinned 마스크용
	ProgramPtr m_postProgram;		 // 합성용
	glm::vec3 m_color  { 1.0f };
	float m_thickness  { 2.0f };
};