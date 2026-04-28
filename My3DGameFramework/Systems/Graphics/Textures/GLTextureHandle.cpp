#include "GraphicsPch.h"
#include "GLTextureHandle.h"
#include "Managers/TypeManager.h"

namespace MGF3D
{
	GLTextureHandle::GLTextureHandle() = default;
	GLTextureHandle::~GLTextureHandle()
	{
		if (m_handle != 0)
		{
			glDeleteTextures(1, &m_handle);
			m_handle = 0;
			m_target = 0;
		}
	}

	/*==========================//
	//   GLTextureHandle Type   //
	//==========================*/
	int16 GLTextureHandle::s_typeIndex = -1;
	const MGFType* GLTextureHandle::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Resource");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	void GLTextureHandle::Bind(uint32 slot) const
	{
		if (m_handle == 0) return;
		glBindTextureUnit(slot, m_handle);
	}

	void GLTextureHandle::Unbind(uint32 slot)
	{
		glBindTextureUnit(slot, 0);
	}

	uint32 GLTextureHandle::GetTarget() const
	{
		return m_target;
	}

	uint32 GLTextureHandle::GetHandle() const
	{
		return m_handle;
	}

	void GLTextureHandle::GenerateMipmap()
	{
		if (m_handle == 0) return;
		glGenerateTextureMipmap(m_handle);
		glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
}