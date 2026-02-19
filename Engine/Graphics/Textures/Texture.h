#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Textures/TextureBase.h"
#include <ktx.h>

CLASS_PTR(Texture)
class Texture : public TextureBase
{
	DEFINE_RESOURCE_TYPE(ResourceType::Texture, TextureDesc)

public:
	virtual ~Texture();
	static TexturePtr Load(const TextureDesc& desc);
	static TexturePtr Create
	(
		int32 width, int32 height, 
		uint32 internalFormat, uint32 format, 
		uint32 type
	);
	static TexturePtr CreateSolid(const std::vector<uint8>& rgba);
	static TexturePtr GetWhiteTexture();
	static TexturePtr GetBlackTexture();
	static TexturePtr GetBlueTexture();
	static TexturePtr GetGrayTexture();

/*=========================//
//   resource descriptor   //
//=========================*/
public:
	virtual TextureDesc& GetDesc() override { return m_desc; }
	virtual const ResourceDesc& GetDesc() const override { return m_desc; }

/*================================//
//   default texture operations   //
//================================*/
public:
	void Resize(int32 width, int32 height);
	void SetFilter(uint32 minFilter, uint32 magFilter) const;
	void SetWrap(uint32 sWrap, uint32 tWrap) const;
	void SetTextureFormat(int32 width, int32 height, uint32 internalFormat, uint32 format, uint32 type);
	void SetBorderColor(const glm::vec4& color) const;
	void SetSubData(int32 x, int32 y, int32 width, int32 height, const void* data);
	void SetData(const void* data, uint32 size = 0);
	/*void GenerateMipmap() const;
	void Bind() const;*/
	
private:
	Texture();
	bool InitFromKtx(const std::string& path);
	void InitProcedural
	(
		int32 width, int32 height, 
		uint32 internalFormat, uint32 format, 
		uint32 type
	);
	void UploadKtxData(ktxTexture2* kTexture);
	TextureDesc m_desc;

/*====================================//
//   Texture default static methods   //
//====================================*/
private:
	static TexturePtr s_white;
	static TexturePtr s_black;
	static TexturePtr s_blue;
	static TexturePtr s_gray;
};