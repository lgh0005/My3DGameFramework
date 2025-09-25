#include "pch.h"
#include "Pyramid.h"
#include "Engine/VertexData.h"
#include "Engine/VertexArray.h"
#include "Engine/IndexBuffer.h"

#include "Engine/Mesh.h"
#include "Engine/Shader.h"
#include "Engine/Pass.h"

Pyramid::Pyramid(const string& name) : Super(name)
{

}

void Pyramid::Init()
{
	Super::Init();
	
	// Create Pyramid Model
	_pyramid = make_shared<Mesh>("Pyramid");
	_pyramid->CreateSphere();

	// Shader
	_shader = std::make_shared<Shader>("shader2.vert", "shader2.frag");
	_shader->AddUniform("model", _model);

	// Render pass
	_pass = make_shared<Pass>();
	_pass->_shader = _shader;
}

void Pyramid::Update()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update Logic
	_rotationAngle += 0.01f; 
	_model = glm::rotate(glm::mat4(1.0f), _rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
	_shader->SetUniformValue("model", _model);

	// Render pass
	_pass->Draw(_pyramid->GetVertexArray()->GetVAO(), _pyramid->GetIndexBuffer()->GetCount());

	SDL_GL_SwapWindow(_window);
}