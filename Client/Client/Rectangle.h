#pragma once
#include "Engine\Scene.h"

class Rectangle : public Scene
{
	using Super = Scene;

public:
	Rectangle(const string& name);
	virtual ~Rectangle() override = default;

public:
	virtual void Init() override;
	virtual void Update() override;

private:
	void CreateRectangle();

private:
	string ReadFile(const string& filePath);
	void AddShader(const string& shaderCode, GLenum shaderType);
	void CompileShader();

private:
	GLuint _vao;
	GLuint _ibo;
	GLuint _vbo;
	GLuint _shader;

private:
	string VertexShaderFilePath = "shader.vert";
	string FragmentShaderFilePath = "shader.frag";

};

