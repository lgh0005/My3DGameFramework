#include "pch.h"
#include "CameraTest.h"
#include "Engine/TempCamera.h"

CameraTest::CameraTest(const string& name) : Super(name)
{

}

void CameraTest::Init()
{
	Super::Init();
	CreateTexturedPyramid();
	CompileShader();
	AddUniforms();
	LoadTexture();
	
	// Create temp camera
	glm::mat4 view = glm::lookAt
	(
		glm::vec3(0.f, 0.f, 5.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f)
	);

	glm::mat4 projection = glm::perspective
	(
		glm::radians(45.f),
		(float)WindowConfig::GWinSizeX / WindowConfig::GWinSizeY,
		0.1f,           
		100.f
	);
	_camera = make_shared<TempCamera>(view, projection);
}

void CameraTest::Update()
{
	// Update camera
	_camera->Update();

	// Increase angle
	_angle += 0.5f;

	/* Render Logic */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(_shader);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	// SRT
	_model = glm::mat4(1.0f);
	_model = glm::rotate(_model, glm::radians(_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	_model = glm::scale(_model, glm::vec3(0.5f, 0.5f, 0.5f));

	// Set MVP matrix variables
	glUniformMatrix4fv(_uniformModel, 1, GL_FALSE, glm::value_ptr(_model));
	glUniformMatrix4fv(_uniformView, 1, GL_FALSE, glm::value_ptr(_camera->GetView()));
	glUniformMatrix4fv(_uniformProjection, 1, GL_FALSE, glm::value_ptr(_camera->GetProjection()));

	// Use texture
	UseTexture();
	glUniform1i(_uniformSampler, 0);

	// Draw pyramid (indexed)
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	SDL_GL_SwapWindow(_window);
}

void CameraTest::CreateTexturedPyramid()
{
	int32 indices[] =
	{
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] =
	{
		// vertices Position	// UV
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		0.0f,  -1.0f, 2.0f,		0.5f, 0.0f,
		1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,		0.5f, 1.0f,
	};

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Attrib #1 : vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)0);
	glEnableVertexAttribArray(0);

	// Attrib #2 : uv
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

string CameraTest::ReadFile(const string& filePath)
{
	// Open shader file
	string content;
	ifstream fileStream(filePath.c_str(), ios::in);

	// Read shader file
	string line = "";
	while (!fileStream.eof())
	{
		getline(fileStream, line);
		content.append(line + "\n");
	}

	// Return shader code
	fileStream.close();
	return content;
}

void CameraTest::AddShader(const string& shaderCode, GLenum shaderType)
{
	// Create Shader
	GLuint shader = glCreateShader(shaderType);

	// Compile Shader
	const GLchar* code = shaderCode.c_str();
	GLint codeLen = (GLint)shaderCode.size();
	glShaderSource(shader, 1, &code, &codeLen);
	glCompileShader(shader);

	// Check compile error
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint logLength = 0; char log[1024];
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		glGetShaderInfoLog(shader, logLength, nullptr, log);
	}

	// Attach shader to shader program.
	glAttachShader(_shader, shader);
}

void CameraTest::CompileShader()
{
	// Create shader program
	_shader = glCreateProgram();

	// Add shader and compile it
	AddShader(ReadFile(VertexShaderFilePath), GL_VERTEX_SHADER);
	AddShader(ReadFile(FragmentShaderFilePath), GL_FRAGMENT_SHADER);

	// Link shader program
	GLint success = 0;
	glLinkProgram(_shader);
	glGetProgramiv(_shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) return;

	// Validate shader program
	glValidateProgram(_shader);
	glGetProgramiv(_shader, GL_VALIDATE_STATUS, &success);
	if (success == GL_FALSE) return;
}

void CameraTest::AddUniforms()
{
	// Mesh
	_uniformModel = glGetUniformLocation(_shader, "model");
	_uniformView = glGetUniformLocation(_shader, "view");
	_uniformProjection = glGetUniformLocation(_shader, "projection");

	// Texture
	_uniformSampler = glGetUniformLocation(_shader, "textureSampler");
}

void CameraTest::LoadTexture()
{
	SDL_Surface* surface = IMG_Load(_textureFilePath.c_str());
	if (surface == nullptr) return;

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(surface);
}

void CameraTest::UseTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
}

void CameraTest::ClearTexture()
{
	glDeleteTextures(1, &_texture);
}

