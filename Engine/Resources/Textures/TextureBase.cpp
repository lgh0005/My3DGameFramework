#include "EnginePch.h"
#include "TextureBase.h"

TextureBase::TextureBase() = default;
TextureBase::~TextureBase()
{
	if (m_texture)
	{
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}

void TextureBase::Bind() const
{
	glBindTexture(m_target, m_texture);
}

void TextureBase::Unbind() const
{
	glBindTexture(m_target, 0);
}
