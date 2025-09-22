#pragma once
#include "Engine\Scene.h"

class Pyramid : public Scene
{
	using Super = Scene;

public:
	Pyramid(const string& name);
	virtual ~Pyramid() override = default;

public:
	virtual void Init() override;
	virtual void Update() override;

private:
	void CreatePyramid();

private:
	string ReadFile(const string& filePath);
	void AddShader(const string& shaderCode, GLenum shaderType);
	void CompileShader();
	void AddUniforms();

private:
	GLuint _vao;
	GLuint _ibo;
	GLuint _vbo;
	GLuint _shader;

private:
	glm::mat4 _model = glm::mat4(1.0f);
	float _angle = 0.f;

private:
	GLuint _uniformModel;

private:
	string VertexShaderFilePath = "shader2.vert";
	string FragmentShaderFilePath = "shader2.frag";
};

