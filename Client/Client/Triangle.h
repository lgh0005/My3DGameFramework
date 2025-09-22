#pragma once
#include "Engine\Scene.h"

class Triangle : public Scene
{
	using Super = Scene;

public:
	Triangle(const string& name);
	virtual ~Triangle() override = default;

public:
	virtual void Init() override;
	virtual void Update() override;

private:
	void CreateTriangle();

private:
	string ReadFile(const string& filePath);
	void AddShader(const string& shaderCode, GLenum shaderType);
	void CompileShader();

private:
	GLuint _vao;
	GLuint _vbo;
	GLuint _shader;

private:
	string VertexShaderFilePath = "shader.vert";
	string FragmentShaderFilePath = "shader.frag";
};

