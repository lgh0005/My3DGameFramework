#pragma once
#include "Asset/Asset.h"
#include "Shader/ShaderDescriptor.h"

namespace MGF3D
{
	// TODO : 얘네들 정적 팩토리 메서드로 바꿔야함.
	MGF_CLASS_PTR(Shader)
	class Shader : public Asset
	{
		using Super = Asset;
		
	/*==================================//
	//         Type System              //
	//==================================*/
	public:
		static UniquePtr<MGFType> s_type;
		static void InitializeType();
		virtual const MGFType* GetType() const override;

	/*==================================//
	//         Asset Logic              //
	//==================================*/
	public:
		static ShaderPtr Create(ShaderDescriptor&& desc);
		virtual ~Shader() override;
		usize GetShader() const { return m_shader; }

	public:
		virtual bool OnLoad() override;
		virtual bool OnCommit() override;
		virtual void OnRelease() override;

	private:
		Shader(const ShaderDescriptor& desc);
		bool Init(ShaderDescriptor&& desc);

		GLenum  m_type    { GL_NONE };
		bool    m_isSpirv { false };
		SString m_entryPoint;

		// 임시 로드 데이터 (Worker -> Main 전달용)
		SString m_sourceCode;
		SVector<byte> m_binaryData;

		// OpenGL 핸들
		uint32 m_shader{ 0 };
	};
}