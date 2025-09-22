#include "pch.h"
#include "Triangle.h"

Triangle::Triangle(const string& name) : Super(name)
{

}

void Triangle::Init()
{
	Super::Init();
	CreateTriangle();
	CompileShader();
}

void Triangle::Update()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(_shader);
	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glUseProgram(0);

	SDL_GL_SwapWindow(_window);
}

void Triangle::CreateTriangle()
{
	GLfloat vertices[] = 
	{
		// Vertices				// Color
		-0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,
		 0.0f, 0.5f, 0.0f,		0.0f, 0.0f, 1.0f
	};

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

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
	glBindVertexArray(0);
}

string Triangle::ReadFile(const string& filePath)
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

void Triangle::AddShader(const string& shaderCode, GLenum shaderType)
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

void Triangle::CompileShader()
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


