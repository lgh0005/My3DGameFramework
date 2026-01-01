#pragma once
#include "Graphics/GlobalUniforms.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(Uniformbuffer)
#pragma endregion

CLASS_PTR(StandardGlobalUniforms)
class StandardGlobalUniforms : public GlobalUniforms
{
public:
	virtual ~StandardGlobalUniforms();
	static StandardGlobalUniformsUPtr Create();
	virtual void PreRender(RenderContext* context) override;

/*==============================//
//   UBO update helper methods  //
//==============================*/
private:
	void UpdateCameraUBO(Camera* camera);
	void UpdateLightUBO
	(
		const std::vector<Light*>& lights, 
		const glm::vec3& viewPos
	);

private:
	StandardGlobalUniforms();
	bool Init();

	UniformbufferUPtr m_cameraUBO;
	UniformbufferUPtr m_lightUBO;
};