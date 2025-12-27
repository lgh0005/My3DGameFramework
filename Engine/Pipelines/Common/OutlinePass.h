#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(OutlinePass)
class OutlinePass : public ContextRenderPass
{
public:
	virtual ~OutlinePass();
	static OutlinePassUPtr Create();
	virtual void Render(RenderContext* context) override;

/*==============================//
//   outlining helper methods   //
//==============================*/
private:
	void MaskMeshes(const std::vector<MeshOutline*>& outlines);
	void DrawMeshes(const std::vector<MeshOutline*>& outlines);
	void MaskStaticMeshes(const std::vector<MeshOutline*>& outlines);
	void MaskSkinnedMeshes(const std::vector<MeshOutline*>& outlines);
	void DrawStaticMeshOutlines(const std::vector<MeshOutline*>& outlines);
	void DrawSkinnedMeshOutlines(const std::vector<MeshOutline*>& outlines);

private:
	OutlinePass();
	bool Init();

	ProgramUPtr m_staticProgram;
	ProgramUPtr m_skinnedProgram;
};