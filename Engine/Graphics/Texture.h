#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Image)
#pragma endregion

CLASS_PTR(Texture)
class Texture
{
public:
	static TextureUPtr Create(int32 width, int32 height, 
						      uint32 format, uint32 type = GL_UNSIGNED_BYTE);
	static TextureUPtr CreateFromImage(const Image* image);
	~Texture();

	const uint32 Get() const	{ return m_texture; }
	void Bind() const;
	void SetFilter(uint32 minFilter, uint32 magFilter) const;
	void SetWrap(uint32 sWrap, uint32 tWrap) const;
	void SetTextureFormat(int32 width, int32 height,
						  uint32 format, uint32 type);
	void SetBorderColor(const glm::vec4& color) const;

	int32  GetWidth()	   const	{ return m_width; }
	int32  GetHeight()	   const	{ return m_height; }
	uint32 GetType()	   const    { return m_type; }
	uint32 GetFormat()     const	{ return m_format; }

private:
	Texture() = default;
	void CreateTexture();
	void SetTextureFromImage(const Image* image);
	void SetTextureFormat(int32 width, int32 height, uint32 format);

	uint32	m_texture		{ 0 };
	int32	m_width			{ 0 };
	int32	m_height		{ 0 };
	uint32  m_type			{ GL_UNSIGNED_BYTE };
	uint32	m_format		{ GL_RGBA };
};