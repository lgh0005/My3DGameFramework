#pragma once

namespace MGF3D
{
	enum class EShaderFileType
	{
		GLSL,
		SPIRV
	};

	MGF_CLASS_PTR(Shader)
	class Shader
	{
	public:
		~Shader();
		static ShaderPtr Create
		(
			const String& filename, GLenum shaderType,
			EShaderFileType shaderFileType, const String& entryPoint = "main"
		);
		uint32 GetHandle() const { return m_handle; }

	private:
		Shader();

		bool LoadGLSL(const String& filename, GLenum shaderType);
		bool LoadSpirV(const String& filename, GLenum shaderType, const String& entryPoint);

		uint32 m_handle { 0 };
	};
}