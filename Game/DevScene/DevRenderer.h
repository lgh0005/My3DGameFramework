#pragma once
#include "Core/Renderer.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Framebuffer)
#pragma endregion

// TODO 
// 1. 프레임 버퍼 기반 렌더링

CLASS_PTR(DevRenderer)
class DevRenderer : public Renderer
{
public:
	static DevRendererUPtr Create(int32 width, int32 height);
	virtual void Render(Scene* scene) override;

private:
	DevRenderer() = default;
	virtual bool Init(int32 width, int32 height) override;

private:
	ProgramUPtr m_program;
	ProgramUPtr m_simpleProgram;
	ProgramUPtr m_lighting2;
	ProgramUPtr m_skinningProgram;		// 조명 영향 없는 스키닝 셰이더
	ProgramUPtr m_skinningLightProgram; // 조명 영향을 받는 스키닝 셰이더

	// --- 후처리(Post-Processing) 리소스 ---
	FramebufferUPtr m_frameBuffer; 
	MeshUPtr		m_plane;       
	ProgramUPtr		m_postProgram; 
	float			m_gamma{ 0.85f };

	// [추가] 셰이더가 m_lightMaterial을 사용하므로 임시로 추가
	MaterialPtr m_lightMaterial;
};