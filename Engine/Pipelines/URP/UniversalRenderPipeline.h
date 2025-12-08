#pragma once
#include "Core/RenderPipeline.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(CullingPass)
CLASS_PTR(UniversalGlobalUniforms)
#pragma endregion

CLASS_PTR(UniversalRenderPipeline)
class UniversalRenderPipeline : public RenderPipeline
{
public:
	static UniversalRenderPipelineUPtr Create();
	virtual void Render(Scene* scene) override;
	virtual void OnResize(int32 width, int32 height) override;

private:
	UniversalRenderPipeline() = default;
	bool Init();

	// TODO : PBR 기반 렌더링 파이프라인
	// SRP에 쓰였던 일부 렌더링 파이프라인을 재사용하거나 조금 수정
	// 필요할 수 있음.

	// 공통 필수 렌더 패스
	CullingPassUPtr		      m_cullingPass			{ nullptr };

	// URP 필수 렌더 패스

	// URP 공통 유니폼 변수 버퍼
	UniversalGlobalUniformsUPtr m_globalUniforms	{ nullptr }; 


/*================================//
//   IMGUI debug context method   //
//================================*/
private:
	void RenderIMGUIContext();
};