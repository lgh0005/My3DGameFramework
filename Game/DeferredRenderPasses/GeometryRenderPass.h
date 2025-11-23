#pragma once
#include "Core/RenderPass.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(Framebuffer)
#pragma endregion

CLASS_PTR(GeometryRenderPass)
class GeometryRenderPass : public GeometryPass
{
public:
	static GeometryRenderPassUPtr Create
	(
		ProgramUPtr staticGeometryProgram,
		ProgramUPtr skinnedGeometryProgram,
		int32 width, int32 height
	);

	virtual void Render(Scene* scene, Camera* camera) override;

private:
	GeometryRenderPass() = default;
	bool Init
	(
		ProgramUPtr staticGeometryProgram,
		ProgramUPtr skinnedGeometryProgram,
		int32 width, int32 height
	);

	ProgramUPtr m_staticGeometryProgram;
	ProgramUPtr m_skinnedGeometryProgram;
};