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

	UniformbufferUPtr m_cameraUBO;
	UniformbufferUPtr m_lightUBO;
};