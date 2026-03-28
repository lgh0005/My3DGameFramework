#pragma once
#include "Asset/IAssetDescriptor.h"

namespace MGF3D
{
	MGF_STRUCT_PTR(ShaderDescriptor)
	struct ShaderDescriptor : public IAssetDescriptor
	{
		using Super = IAssetDescriptor;

		ShaderDescriptor
		(
			const Ptr<MGFType> _type,
			const MGFName& _name,
			const MGFPath& _path,
			const SString& _ext,
			GLenum _shaderType,
			bool _isSpirv = false,
			const SString& _entryPoint = "main"
		);
		virtual ~ShaderDescriptor() override;

		GLenum  shaderType;
		bool	isSpirv;
		SString entryPoint;
	};
}