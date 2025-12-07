#include "EnginePch.h"
#include "UniversalGlobalUniforms.h"
#include "Core/RenderContext.h"
#include "Graphics/UniformBuffer.h"

UniversalGlobalUniformsUPtr UniversalGlobalUniforms::Create()
{
	auto globalUniforms = UniversalGlobalUniformsUPtr(new UniversalGlobalUniforms());
	if (!globalUniforms->Init()) return nullptr;
	return std::move(globalUniforms);
}

bool UniversalGlobalUniforms::Init()
{
	return true;
}

void PreRender(RenderContext* context)
{

}
