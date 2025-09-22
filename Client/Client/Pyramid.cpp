#include "pch.h"
#include "Pyramid.h"

Pyramid::Pyramid(const string& name) : Super(name)
{

}

void Pyramid::Init()
{
	Super::Init();
	CreatePyramid();
	CompileShader();
	AddUniforms();
}

void Pyramid::Update()
{
	// Increase angle
	_angle += 0.5f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(_shader);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	// SRT
	_model = glm::mat4(1.0f);
	_model = glm::rotate(_model, glm::radians(_angle), glm::vec3(0.0f, 1.0f, 0.0f));
	_model = glm::scale(_model, glm::vec3(0.5f, 0.5f, 0.5f));

	// Set value uniform variable : "model"
	glUniformMatrix4fv(_uniformModel, 1, GL_FALSE, glm::value_ptr(_model));

	// Draw pyramid (indexed)
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	SDL_GL_SwapWindow(_window);
}

void Pyramid::CreatePyramid()
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
		// Vertex				// Color
		-1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f,   // »¡°­
		0.0f,  -1.0f, 1.5f,  1.0f, 1.0f, 0.0f,   // ³ë¶û : ±íÀÌ ºÎ¿©.
		1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   // ÃÊ·Ï
		0.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   // ÆÄ¶û
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (void*)0);
	glEnableVertexAttribArray(0);

	// Attrib #2 : color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (void*)(sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

string Pyramid::ReadFile(const string& filePath)
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

void Pyramid::AddShader(const string& shaderCode, GLenum shaderType)
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

void Pyramid::CompileShader()
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
	if (success == GL_FALSE)
	{
		GLint logLength = 0; GLchar log[1024];
		glGetProgramiv(_shader, GL_INFO_LOG_LENGTH, &logLength);
		glGetProgramInfoLog(_shader, logLength, nullptr, log);
	}

	// Validate shader program
	glValidateProgram(_shader);
	glGetProgramiv(_shader, GL_VALIDATE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint logLength = 0; GLchar log[1024];
		glGetProgramiv(_shader, GL_INFO_LOG_LENGTH, &logLength);
		glGetProgramInfoLog(_shader, logLength, nullptr, log);
	}
}

void Pyramid::AddUniforms()
{
	_uniformModel = glGetUniformLocation(_shader, "model");
}
