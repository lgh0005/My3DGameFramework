#pragma once
#include "Core/Renderer.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Framebuffer)
CLASS_PTR(CubeTexture)
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
	ProgramUPtr m_grassInstancing;	    // 인스턴싱 테스트

	// --- 후처리(Post-Processing) 리소스 ---
	FramebufferUPtr m_frameBuffer; 
	MeshUPtr		m_plane;       
	ProgramUPtr		m_postProgram; 
	float			m_gamma{ 0.65f };

	/// <summary>
	/// 스카이 및 환경 박스
	/// </summary>
	MeshUPtr		m_box;
	// --- 스카이 박스 리소스 ---
	CubeTextureUPtr m_cubeTexture;
	ProgramUPtr		m_skyboxProgram;

	// --- 환경맵 리소스 ---
	ProgramUPtr m_envMapProgram;
};