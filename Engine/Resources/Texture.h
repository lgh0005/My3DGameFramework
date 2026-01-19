#pragma once
#include "Resources/Resource.h"
#include <ktx.h>

#pragma region FORWARD_DECLARATION
CLASS_PTR(Image)
#pragma endregion

// TODO : 이것도 Texture를 상위 클래스로 두고 전용 텍스쳐로
// 조금 나눌 필요가 있어보인다.
CLASS_PTR(Texture)
class Texture : public Resource
{
public:
	virtual ~Texture();
	static const ResourceType s_ResourceType = ResourceType::Texture;
	virtual ResourceType GetResourceType() const override { return ResourceType::Texture; }
	static TextureUPtr Create(int32 width, int32 height, uint32 internalFormat, uint32 format, uint32 type);
	static TextureUPtr CreateFromImage(const Image* image);
	static TextureUPtr CreateFromHDR(const Image* image);
	static TextureUPtr CreateFromKtxImage(const std::string& ktxFilePath);
	static TextureUPtr CreateFromKtxHDR(const std::string& ktxFilePath);
	static TextureUPtr CreateMultisample(int32 width, int32 height, int32 samples, uint32 internalFormat);

public:
	const uint32 Get() const	{ return m_texture; }
	void Bind() const;
	void SetFilter(uint32 minFilter, uint32 magFilter) const;
	void SetWrap(uint32 sWrap, uint32 tWrap) const;
	void SetTextureFormat(int32 width, int32 height, uint32 internalFormat, uint32 format, uint32 type);
	void SetBorderColor(const glm::vec4& color) const;
	void SetSubData(int32 x, int32 y, int32 width, int32 height, const void* data);
	void SetData(const void* data, uint32 size = 0);

	int32  GetWidth()	   const	{ return m_width; }
	int32  GetHeight()	   const	{ return m_height; }
	uint32 GetType()	   const    { return m_type; }
	uint32 GetFormat()     const	{ return m_format; }

	int32 GetSamples() const { return m_samples; }

	void Resize(int32 width, int32 height);

private:
	Texture();
	void CreateTexture();
	void SetTextureFromImage(const Image* image);
	static TextureUPtr LoadKtx(const std::string& ktxFilePath);
	static TexturePtr Create4x4Texture(const std::vector<uint8>& colorData);

	uint32	m_texture		{ 0 };
	int32	m_width			{ 0 };
	int32	m_height		{ 0 };
	uint32  m_internalFormat{ 0 };
	int32   m_samples		{ 1 };
	uint32  m_type			{ GL_UNSIGNED_BYTE };
	uint32	m_format		{ GL_RGBA };
	GLenum  m_target		{ GL_TEXTURE_2D };

/*===================//
//   texture utils   //
//===================*/
public:
	static TexturePtr CreateWhite();
	static TexturePtr CreateGray();
	static TexturePtr CreateBlack();
	static TexturePtr CreateBlue();

private:
	static TexturePtr s_whiteTex;
	static TexturePtr s_grayTex;
	static TexturePtr s_blackTex;
	static TexturePtr s_blueTex;
};