#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Framebuffer)
CLASS_PTR(ScreenMesh)
#pragma endregion

CLASS_PTR(PostProcessEffect)
class PostProcessEffect
{
public:
	virtual ~PostProcessEffect();
	virtual bool Render
	(
		RenderContext* context,
		Framebuffer* srcFBO,
		Framebuffer* dstFBO,
		ScreenMesh* screenMesh
	) = 0;
	virtual void OnResize(int32 width, int32 height);

	void SetEnable(bool enable) { m_isEnable = enable; }
	bool IsEffectEnable() const { return m_isEnable; }

	int32 GetPriority() const { return m_priority; }
	void SetPriority(int32 priority) { m_priority = priority; }

protected:
	PostProcessEffect();

	bool  m_isEnable{ true };
	int32 m_priority{ 0 };    // 실행 순서 (낮을수록 먼저 실행)
	
	// 해상도 정보 캐싱 (Resize나 FBO 생성 시 필요)
	int32 m_width{ 0 };
	int32 m_height{ 0 };
};