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
		GLShader();
		virtual ~GLShader() override;
		virtual bool OnSyncCreate() override;
		uint32 GetHandle() const { return m_handle; }

	public:
		void SetSourceGLSL(GLenum type, const String& source);
		void SetSourceSpirv(GLenum type, Vector<char>&& binary, const String& entryPoint);

	private:
		uint32 m_handle{ 0 };
		GLenum m_type{ 0 };

		bool m_isSpirv{ false };
		String m_sourceGLSL;
		Vector<char> m_sourceSpirv;
		String m_entryPoint;
	};
}