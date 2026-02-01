#pragma once
#include "Pipelines/PostProcessing/PostProcessEffect.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Program)
CLASS_PTR(OutlineFramebuffer)
CLASS_PTR(RenderQueue)
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
		Framebuffer* srcFBO,
		Framebuffer* dstFBO,
		ScreenMesh* screenMesh
	) override;

	virtual void OnResize(int32 width, int32 height) override;

private:
	OutlineEffect();
	bool Init
	(
		int32 priority, int32 width, int32 height,
		const glm::vec3& color, float thickness
	);
	void CollectOutlines(const std::vector<class MeshOutline*>& outlines);

	OutlineFramebufferUPtr m_maskFBO;
	ProgramPtr m_maskInstancedProgram; // 메쉬 마스크용
	ProgramPtr m_postProgram;		   // 합성용
	RenderQueueUPtr m_renderQueue;

	glm::vec3 m_color  { 1.0f };
	float m_thickness  { 2.0f };
};