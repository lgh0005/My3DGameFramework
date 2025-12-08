#pragma once
#include "Core/GlobalUniforms.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Uniformbuffer)
#pragma endregion

CLASS_PTR(UniversalGlobalUniforms)
class UniversalGlobalUniforms : public GlobalUniforms
{
public:
	static UniversalGlobalUniformsUPtr Create();
	virtual ~UniversalGlobalUniforms();
	virtual void PreRender(RenderContext* context) override;

private:
	UniversalGlobalUniforms();
	bool Init();

	UniformbufferUPtr m_cameraUBO;
	UniformbufferUPtr m_lightUBO;
};