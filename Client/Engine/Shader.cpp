#include "pch.h"
#include "Shader.h"

Shader::Shader(const string& vertPath, const string& fragPath)
	: _vertexShaderFilePath(vertPath), _fragmentShaderFilePath(fragPath)
{
	CompileShader();
}

void Shader::AddUniform(const string& uniformName)
{
	_uniformLocation[uniformName] = glGetUniformLocation(_shader, uniformName.c_str());
}

GLuint Shader::GetUniformLocation(const string& uniformName)
{
	auto it = _uniformLocation.find(uniformName);
	if (it != _uniformLocation.end()) return it->second;
	return -1;
}

string Shader::ReadFile(const string& filePath)
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

void Shader::AddShader(const string& shaderCode, GLenum shaderType)
{
	// Create Shader
	GLuint shader = glCreateShader(shaderType);

	// Compile Shader
	const GLchar* code = shaderCode.c_str();
	GLint codeLen = (GLint)shaderCode.size();
	glShaderSource(shader, 1, &code, &codeLen);
	glCompileShader(shader);

	// TODO : Check compile error

	// Attach shader to shader program.
	glAttachShader(_shader, shader);
}

void Shader::CompileShader()
{
	// Create shader program
	_shader = glCreateProgram();

	// Add shader and compile it
	AddShader(ReadFile(_vertexShaderFilePath), GL_VERTEX_SHADER);
	AddShader(ReadFile(_fragmentShaderFilePath), GL_FRAGMENT_SHADER);

	// TODO : 오류를 출력하는 부분을 수정할 필요가 있다.
	// Link shader program
	GLint success = 0;
	glLinkProgram(_shader);

	// Validate shader program
	glValidateProgram(_shader);
}
