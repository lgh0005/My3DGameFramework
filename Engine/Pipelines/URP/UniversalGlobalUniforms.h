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
	virtual ~UniversalGlobalUniforms() = default;
	virtual void PreRender(RenderContext* context) override;

private:
	UniversalGlobalUniforms();
	bool Init();

	// TODO : SRP와 다르게 추가해야 할 ubo가 뭔지 고려 필요
	UniformbufferUPtr m_cameraUBO;
	UniformbufferUPtr m_lightUBO;
};