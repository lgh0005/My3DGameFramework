#pragma once
#include "Sources/Asset.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLShader)

	enum class EShaderFileType
	{
		GLSL,
		SPIRV
	};

	MGF_CLASS_PTR(Shader)
	class Shader : public Asset
	{
		MGF_DISABLE_COPY(Shader);
		using Super = Asset;

	public:
		Shader
		(
			const String& path, GLenum shaderType, 
			EShaderFileType fileType, const String& entryPoint = "main");
		virtual ~Shader() override;

	/*========================//
	//       Shader Type      //
	//========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual bool Load() override;
		GLenum GetShaderType() const { return m_shaderType; }
		EShaderFileType GetFileType() const { return m_fileType; }

	private:
		bool LoadGLSL(const GLShaderPtr& module);
		bool LoadSpirV(const GLShaderPtr& module);

	private:
		GLenum m_shaderType;
		EShaderFileType m_fileType;
		String m_entryPoint;
	};
}