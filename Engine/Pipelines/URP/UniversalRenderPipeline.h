#pragma once
#include "Core/RenderPipeline.h"

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

/*================================//
//   IMGUI debug context method   //
//================================*/
private:
	void RenderIMGUIContext();
};