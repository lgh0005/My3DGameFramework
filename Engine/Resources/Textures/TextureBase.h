#pragma once
#include "Resources/Resource.h"
#include "Resources/Textures/TextureUtils.h"

CLASS_PTR(TextureBase)
class TextureBase : public Resource
{
public:
	virtual ~TextureBase();

	const uint32 Get() const { return m_texture; }
	void Bind() const;
	void Unbind() const;

	int32  GetWidth()    const { return m_width; }
	int32  GetHeight()   const { return m_height; }
	uint32 GetFormat()   const { return m_format; } // Upload/Data Format (GL_RGBA etc)
	uint32 GetType()     const { return m_type; }   // Data Type (GL_UNSIGNED_BYTE etc)
	uint32 GetInternalFormat() const { return m_internalFormat; }

protected:
	TextureBase();

	uint32 m_texture{ 0 };
	GLenum m_target{ 0 };

	int32  m_width{ 0 };
	int32  m_height{ 0 };

	uint32 m_internalFormat{ 0 };
	uint32 m_format{ 0 };
	uint32 m_type{ 0 };
};