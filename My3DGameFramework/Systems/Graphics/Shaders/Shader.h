#pragma once
#include "Pointer/RefCount.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Shader)
	class Shader : public RefCount
	{
	public:
		virtual ~Shader() override;

	/*===================================//
	//   Shader loading asynchronously   //
	//===================================*/
	public:
		static void CreateFromTextFileAsync
		(
			const MGFPath& filepath, 
			GLenum shaderType, 
			Action<ShaderPtr> onComplete
		);

		static void CreateFromSPIRVAsync
		(
			const MGFPath& filepath, 
			GLenum shaderType, 
			strview entryPoint, 
			Action<ShaderPtr> onComplete
		);

	/*==================================//
	//   Shader loading synchronously   //
	//==================================*/
	public:
		static ShaderPtr CreateFromTextFile(const MGFPath& filepath, GLenum shaderType);
		static ShaderPtr CreateFromSPIRV(const MGFPath& filepath, GLenum shaderType, strview entryPoint = "main");

	public:
		usize GetShader() const { return m_shader; }

	/*===================================//
	//   Shader Initialization methods   //
	//===================================*/
	private:
		Shader();
		bool InitFromTextFile(const MGFPath& filepath, GLenum shaderType);
		bool InitFromSPIRV(const MGFPath& filepath, GLenum shaderType, strview entryPoint);
		
		uint32	m_shader	{ 0 };
		GLenum  m_type		{ GL_NONE };
	};
}