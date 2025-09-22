#pragma once
#include "Engine\Scene.h"

class TempCamera;

class CameraTest : public Scene
{
	using Super = Scene;

public:
	CameraTest(const string& name);
	virtual ~CameraTest() override = default;

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
	float _angle = 0.f;
	
private:
	string VertexShaderFilePath = "shader3.vert";
	string FragmentShaderFilePath = "shader3.frag";

/* =========================
//	  Camera Properties   //
// =======================*/
private:
	shared_ptr<TempCamera> _camera;

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
