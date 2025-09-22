#pragma once
#include "Engine\Scene.h"

class Texture : public Scene
{
	using Super = Scene;

public:
	Texture(const string& name);
	virtual ~Texture() override = default;

public:
	virtual void Init() override;
	virtual void Update() override;

/* ======================= 
//	  Mesh Properties   //
// =====================*/
private:
	void CreateTexturedPyramid();

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
	glm::mat4 _view = glm::lookAt
	(
		glm::vec3(0.0f, 0.0f, 5.0f),  // 카메라 위치
		glm::vec3(0.0f, 0.0f, 0.0f),  // 카메라가 보는 목표
		glm::vec3(0.0f, 1.0f, 0.0f)   // 업 벡터
	);
	glm::mat4 _projection = glm::perspective
	(
		glm::radians(45.0f),										  // FOV
		(float)WindowConfig::GWinSizeX / WindowConfig::GWinSizeY,	  // 화면 비율
		0.1f, 100.0f												  // near / far plane
	);
	float _angle = 0.f;

private:
	string VertexShaderFilePath = "shader3.vert";
	string FragmentShaderFilePath = "shader3.frag";

/* ==========================
//	  Texture Properties   //
// ========================*/
private:
	void LoadTexture();
	void UseTexture();
	void ClearTexture();

private:
	string _textureFilePath = "../Resources/brick.png";

private:
	GLuint _texture;

/* ================
//	  Uniforms   //
// ==============*/
private:
	GLuint _uniformModel;
	GLuint _uniformView;
	GLuint _uniformProjection;
	GLuint _uniformSampler;
};

