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
	usize m_shader	{ 0 };
};

/*================//
//  shader utils  //
//================*/
inline std::optional<std::string> LoadTextFile(const std::string& filename)
{
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        SPDLOG_ERROR("failed to open file: {}", filename);
        return {};
    }
    std::stringstream text;
    text << fin.rdbuf();
    return text.str();
}