#include "GraphicsPch.h"
#include "ShaderDescriptor.h"

namespace MGF3D
{
	ShaderDescriptor::ShaderDescriptor
	(
		const Ptr<MGFType> _type, 
		const MGFName& _name, 
		const MGFPath& _path, 
		const SString& _ext, 
		GLenum _shaderType, 
		bool _isSpirv, 
		const SString& _entryPoint
	) : Super(_type, _name, _path, _ext),
		shaderType(_shaderType), 
		isSpirv(_isSpirv), 
		entryPoint(_entryPoint) { }

	ShaderDescriptor::~ShaderDescriptor() = default;
}