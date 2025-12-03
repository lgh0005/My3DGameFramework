#pragma once
#include "Graphics/Uniform.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Uniformbuffer)
CLASS_PTR(Camera)
CLASS_PTR(Scene)
#pragma endregion

// TODO : 외부에서 필요한 커스텀 UBO가 있다면
// 추가할 수 있도록 마련할 필요가 있음
CLASS_PTR(StandardGlobalUniforms)
class StandardGlobalUniforms
{
public:
	static StandardGlobalUniformsUPtr Create();
	void PreRender(RenderContext* context);
	virtual ~StandardGlobalUniforms();

private:
	StandardGlobalUniforms();
	bool Init();

	UniformbufferUPtr m_cameraUBO;
	UniformbufferUPtr m_lightUBO;
};