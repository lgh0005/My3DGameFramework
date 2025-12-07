#pragma once
#include "Core/GlobalUniforms.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Uniformbuffer)
#pragma endregion

CLASS_PTR(StandardGlobalUniforms)
class StandardGlobalUniforms : public GlobalUniforms
{
public:
	static StandardGlobalUniformsUPtr Create();
	virtual void PreRender(RenderContext* context) override;
	virtual ~StandardGlobalUniforms();

private:
	StandardGlobalUniforms();
	bool Init();

	UniformbufferUPtr m_cameraUBO;
	UniformbufferUPtr m_lightUBO;
};