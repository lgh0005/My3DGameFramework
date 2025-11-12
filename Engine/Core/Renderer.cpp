#include "EnginePch.h"
#include "Renderer.h"

RendererUPtr Renderer::Create(int32 width, int32 height)
{
	auto renderer = RendererUPtr(new Renderer());
	if (!renderer->Init(width, height)) return nullptr;
	return std::move(renderer);
}

void Renderer::Render(Scene* scene)
{
	
}

bool Renderer::Init(int32 width, int32 height)
{
	return false;
}