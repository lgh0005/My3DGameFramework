#pragma once
#include "Pointer/RefCount.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Shader)
	class Shader : public RefCount
	{
	public:
		virtual ~Shader() override;
		static ShaderPtr CreateFromTextFile(const SString& filename, GLenum shaderType);
		static ShaderPtr CreateFromSPIRV(const SString& filename, GLenum shaderType, strview entryPoint = "main");

	public:
		usize GetShader() const { return m_shader; }

	private:
		Nullable<SString> LoadTextFile(const SString& filename);
		Nullable<SVector<uint8>> LoadBinaryFile(const SString& filename);

	private:
		Shader();
		bool InitFromTextFile(const SString& filename, GLenum shaderType);
		bool InitFromSPIRV(const SString& filename, GLenum shaderType, strview entryPoint);
		
		uint32	m_shader	{ 0 };
		GLenum  m_type		{ GL_NONE };
	};
}