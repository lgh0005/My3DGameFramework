#include "EnginePch.h"
#include "JoltDebugRenderer.h"
#include "Physics/JoltGizmo.h"

JoltDebugRenderer::JoltDebugRenderer()
{
	Initialize();
}
JoltDebugRenderer::~JoltDebugRenderer() = default;

JoltDebugRendererUPtr JoltDebugRenderer::Create(JoltGizmo* gizmo)
{
	auto debugger = JoltDebugRendererUPtr(new JoltDebugRenderer());
	debugger->Init(gizmo);
	return std::move(debugger);
}

void JoltDebugRenderer::Init(JoltGizmo* gizmo)
{
	m_gizmo = gizmo;
}

/*======================================//
//  JPH::DebugRenderer implementations  //
//======================================*/
void JoltDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
	if (!m_gizmo) return;

	glm::vec3 start = Utils::ToGlmVec3(inFrom);
	glm::vec3 end = Utils::ToGlmVec3(inTo);
	glm::vec4 color = Utils::ToGlmColor(inColor);

	m_gizmo->DrawLine(start, end, color);
}

void JoltDebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow)
{
	if (!m_gizmo) return;

	glm::vec3 v1 = Utils::ToGlmVec3(inV1);
	glm::vec3 v2 = Utils::ToGlmVec3(inV2);
	glm::vec3 v3 = Utils::ToGlmVec3(inV3);
	glm::vec4 color = Utils::ToGlmColor(inColor);

	m_gizmo->DrawTriangle(v1, v2, v3, color);
}

void JoltDebugRenderer::DrawText3D(JPH::RVec3Arg inPos, const std::string_view& inString, JPH::ColorArg inColor, float inHeight)
{
	// 3D 텍스트는 아직 구현 안 함 (필요시 ImGui 등으로 구현 가능)
	return;
}