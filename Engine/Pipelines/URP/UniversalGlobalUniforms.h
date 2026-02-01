#pragma once
#include "Graphics/Rendering/GlobalUniforms.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(RenderContext)
CLASS_PTR(UniformBuffer)
#pragma endregion

CLASS_PTR(UniversalGlobalUniforms)
class UniversalGlobalUniforms : public GlobalUniforms
{
public:
	virtual ~UniversalGlobalUniforms();
	static UniversalGlobalUniformsUPtr Create();
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
	UniversalGlobalUniforms();
	bool Init();

	UniformBufferUPtr m_cameraUBO;
	UniformBufferUPtr m_lightUBO;
};