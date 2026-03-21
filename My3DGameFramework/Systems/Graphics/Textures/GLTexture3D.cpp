#include "GraphicsPch.h"
#include "GLTexture3D.h"
#include "Utils/TextureUtils.h"

namespace MGF3D
{
	GLTexture3D::GLTexture3D() = default;
	GLTexture3D::~GLTexture3D() = default;

	GLTexture3DPtr GLTexture3D::Create
	(
		int32  width, int32 height, int32 depth,
		uint32 vkFormat, const void* pixels,
		uint32 wrapS, uint32 wrapT, uint32 wrapR,
		uint32 minFilter, uint32 magFilter,
		uint32 handle
	)
	{
		auto texture = GLTexture3DPtr(new GLTexture3D());
		if (!texture->Init
		(
			width, height, depth, 
			vkFormat, pixels, 
			wrapS, wrapT, wrapR, 
			minFilter, magFilter, 
			handle)) return nullptr;
		return texture;
	}

	bool GLTexture3D::Init
	(
		int32  width, int32 height, int32 depth,
		uint32 vkFormat, const void* pixels,
		uint32 wrapS, uint32 wrapT, uint32 wrapR,
		uint32 minFilter, uint32 magFilter,
		uint32 handle
	)
	{
		if (width <= 0 || height <= 0 || depth <= 0) return false;

		m_target = GL_TEXTURE_3D;
		m_width = static_cast<uint32>(width);
		m_height = static_cast<uint32>(height);
		m_depth = static_cast<uint32>(depth);

		uint32 internalFormat = TextureUtils::MapVkFormatToGLInternal(vkFormat);
		uint32 format		  = TextureUtils::GetPixelFormatFromInternal(internalFormat);
		uint32 type			  = TextureUtils::GetGLDataTypeFromVk(vkFormat);

		if (!handle)
		{
			glGenTextures(1, &m_handle);
			if (m_handle == 0) return false;
			glBindTexture(m_target, m_handle);
			glTexImage3D(m_target, 0, internalFormat, m_width, m_height, m_depth, 0, format, type, pixels);
		}
		else
		{
			m_handle = handle;
			glBindTexture(m_target, m_handle);
			if (pixels)
				glTexSubImage3D(m_target, 0, 0, 0, 0, m_width, m_height, m_depth, format, type, pixels);
		}

		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_R, wrapR);
		glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, magFilter);

		glBindTexture(m_target, 0);
		return true;
	}

	void GLTexture3D::Bind(uint32 slot)
	{
		if (m_handle)
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(m_target, m_handle);
		}
	}

	void GLTexture3D::Unbind(uint32 slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(m_target, 0);
	}

	void GLTexture3D::GenerateMipmap()
	{
		if (m_handle == 0) return;

		Bind(0);
		glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(m_target);
		Unbind(0);
	}

	void GLTexture3D::UpdateSubData
	(
		int32 offsetX, int32 offsetY, int32 offsetZ,
		int32 width, int32 height, int32 depth,
		uint32 format, uint32 type,
		const void* pixels
	)
	{
		if (m_handle == 0 || pixels == nullptr) return;

		Bind(0);
		glTexSubImage3D(m_target, 0, offsetX, offsetY, offsetZ, width, height, depth, format, type, pixels);
		Unbind(0);
	}
}