#pragma once
#include "Sources/Resource.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GLShader)
	class GLShader : public Resource
	{
		friend class Shader;

	/*========================//
	//     GLShader Type      //
	//========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	public:
		virtual ~GLShader() override;
		static GLShaderPtr Create(GLenum type, const String& source);
		static GLShaderPtr Create(GLenum type, Vector<char>&& binary, const String& entryPoint = "main");

		virtual bool OnSyncCreate() override;
		uint32 GetHandle() const { return m_handle; }

	private:
		GLShader();
		bool CompileGLSL();
		bool SpecializeSpirv();

		uint32 m_handle{ 0 };
		GLenum m_type{ 0 };

		bool m_isSpirv { false };
		String m_sourceGLSL;
		Vector<char> m_sourceSpirv;
		String m_entryPoint;
	};
}