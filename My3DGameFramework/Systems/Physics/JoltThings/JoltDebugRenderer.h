#pragma once
#include <Jolt/Renderer/DebugRendererSimple.h>

#pragma region FORWARD_DECLARATION
CLASS_PTR(JoltGizmo)
#pragma endregion

CLASS_PTR(JoltDebugRenderer)
class JoltDebugRenderer : public JPH::DebugRendererSimple
{
public:
	virtual ~JoltDebugRenderer() override;
	static JoltDebugRendererUPtr Create(JoltGizmo* gizmo);

/*======================================//
//  JPH::DebugRenderer implementations  //
//======================================*/
public:
	virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
	virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
	virtual void DrawText3D(JPH::RVec3Arg inPos, const std::string_view& inString, JPH::ColorArg inColor, float inHeight) override;

private:
	JoltDebugRenderer();
	void Init(JoltGizmo* gizmo);

	JoltGizmo* m_gizmo = nullptr;
};