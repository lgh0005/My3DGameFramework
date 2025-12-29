#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(OutlineFramebuffer)
CLASS_PTR(ScreenMesh)
#pragma endregion

CLASS_PTR(UniversalOutlinePass)
class UniversalOutlinePass : public ContextRenderPass
{
public:
	virtual ~UniversalOutlinePass();
	static UniversalOutlinePassUPtr Create
	(
		int32 width = WINDOW_WIDTH,
		int32 height = WINDOW_HEIGHT,
		const glm::vec3& color = { 1.0f, 1.0f, 0.5f },
		float thickness = 2.0f
	);
	virtual void Render(RenderContext* context) override;

/*==============================//
//   outlining helper methods   //
//==============================*/
private:
	void MaskMeshes(const std::vector<MeshOutline*>& outlines);
	void MaskStaticMeshes(const std::vector<MeshOutline*>& outlines);
	void MaskSkinnedMeshes(const std::vector<MeshOutline*>& outlines);

private:
	UniversalOutlinePass();
	bool Init
	(
		int32 width, int32 height,
		const glm::vec3& color,
		float thickness
	);

	OutlineFramebufferUPtr m_maskFBO;
	ScreenMeshUPtr m_screenMesh;
	ProgramUPtr m_maskStaticProgram;  // Static 마스크용
	ProgramUPtr m_maskSkinnedProgram; // Skinned 마스크용
	ProgramUPtr m_postProgram;		  // 합성용
	glm::vec3 m_color{ 1.0f };
	float m_thickness{ 2.0f };
};