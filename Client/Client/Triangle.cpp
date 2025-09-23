#include "pch.h"
#include "Triangle.h"
#include "Engine/VertexData.h"

#include "Engine/VertexArray.h"
#include "Engine/VertexBuffer.h"
#include "Engine/IndexBuffer.h"
#include "Engine/Shader.h"
#include "Engine/Pass.h"

Triangle::Triangle(const string& name) : Super(name)
{

}

void Triangle::Init()
{
	Super::Init();
	_vao = make_shared<VertexArray>();
	_vbo = make_shared<VertexBuffer>();
	_ibo = make_shared<IndexBuffer>();
	_shader = std::make_shared<Shader>("shader.vert", "shader.frag");
	_pass = make_shared<Pass>();
	_pass->_shader = _shader;
	
	CreateTriangle();
}

void Triangle::Update()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_pass->Draw(_vao->GetVAO(), _ibo->GetCount());

	SDL_GL_SwapWindow(_window);
}

void Triangle::CreateTriangle()
{
	// Triangle
	std::vector<VertexColorData> vertices =
	{
		{ { -0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ {  0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	vector<GLuint> indices = { 0, 1, 2 };

	_vbo->Create(vertices);
	_vao->AddVertexBuffer(_vbo, { 3, 4 });

	_ibo->Create(indices);
	_vao->SetIndexBuffer(_ibo);
}

