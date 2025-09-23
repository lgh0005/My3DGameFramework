#pragma once

// TODO : 나중에는 그림자를 만드려면 Geometry Shader 사용도 고려해봐야 한다.
class Shader
{
public:
	Shader(const string& vertPath, const string& fragPath);
	~Shader() = default;

public:
	void Bind() const { glUseProgram(_shader); }
	void Unbind() const { glUseProgram(0); }

public:
	void AddUniform(const string& uniformName);
	GLuint GetUniformLocation(const string& uniformName);
	
private:
	string ReadFile(const string& filePath);
	void AddShader(const string& shaderCode, GLenum shaderType);
	void CompileShader();

private:
	GLuint _shader = 0;
	string _vertexShaderFilePath;
	string _fragmentShaderFilePath;
	unordered_map<string, GLuint> _uniformLocation;
};

