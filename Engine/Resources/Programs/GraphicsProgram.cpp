#include "EnginePch.h"
#include "GraphicsProgram.h"
#include "Graphics/Shader.h"

GraphicsProgramUPtr GraphicsProgram::Create(const std::vector<ShaderPtr>& shaders)
{
	// 자식 클래스 인스턴스 생성 (생성자는 private이므로 내부에서 new)
	auto program = GraphicsProgramUPtr(new GraphicsProgram());

	// 부모의 Link 함수 호출
	if (!program->Link(shaders)) return nullptr;

	return std::move(program);
}

GraphicsProgramUPtr GraphicsProgram::Create(const std::string& vertPath, const std::string& fragPath)
{
	// 1. 셰이더 리소스 로드
	ShaderPtr vs = Shader::CreateFromFile(vertPath, GL_VERTEX_SHADER);
	ShaderPtr fs = Shader::CreateFromFile(fragPath, GL_FRAGMENT_SHADER);
	if (!vs || !fs) return nullptr;

	// 2. 벡터로 묶어서 Link 요청
	return Create({ vs, fs });
}