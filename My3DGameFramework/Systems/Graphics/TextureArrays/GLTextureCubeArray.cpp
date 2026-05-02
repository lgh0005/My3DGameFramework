#include "GraphicsPch.h"
#include "GLTextureCubeArray.h"

namespace MGF3D
{
	GLTextureCubeArray::GLTextureCubeArray()
	{
		m_target = GL_TEXTURE_CUBE_MAP_ARRAY;
	}
	GLTextureCubeArray::~GLTextureCubeArray() = default;

	GLTextureCubeArrayUPtr GLTextureCubeArray::Create
	(
		uint32 width, uint32 height, 
		uint32 lightCount
	)
	{
		auto textureArray = GLTextureCubeArrayUPtr(new GLTextureCubeArray());
		if (!textureArray->Init(width, height, lightCount)) return nullptr;
		return textureArray;
	}

	bool GLTextureCubeArray::Init
	(
		uint32 width, uint32 height, 
		uint32 lightCount
	)
	{
		if (width == 0 || height == 0 || lightCount == 0) return false;

		m_width = width;
		m_height = height;
		m_layers = lightCount * 6; // 큐브 하나당 6면

		glCreateTextures(m_target, 1, &m_handle);
		if (m_handle == 0) return false;

		glTextureStorage3D(m_handle, 1, GL_DEPTH_COMPONENT32F, m_width, m_height, m_layers);

		// 큐브맵은 이음새 방지를 위해 CLAMP_TO_EDGE 권장
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return true;
	}
}