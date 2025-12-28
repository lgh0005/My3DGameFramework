#pragma once

CLASS_PTR(Shader)
class Shader
{
public:
	static ShaderUPtr CreateFromFile(const std::string& filename,
		GLenum shaderType);

	usize Get() const { return m_shader; }
	bool LoadFile(const std::string& filename, GLenum shaderType);
	~Shader();

private:
	Shader() = default;
	usize m_shader{ 0 };
};