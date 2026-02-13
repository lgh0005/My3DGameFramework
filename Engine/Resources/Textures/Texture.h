#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Textures/TextureBase.h"
#include "Resources/Textures/TextureUtils.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Image)
#pragma endregion

CLASS_PTR(Texture)
class Texture : public TextureBase
{
	friend class TextureUtils;
	DEFINE_RESOURCE_TYPE(ResourceType::Texture, TextureDesc)

public:
	virtual ~Texture();
	static TexturePtr Load(const TextureDesc& desc);
	static TexturePtr Create(int32 width, int32 height, uint32 internalFormat, uint32 format, uint32 type);

public:
	void Resize(int32 width, int32 height);
	void SetFilter(uint32 minFilter, uint32 magFilter) const;
	void SetWrap(uint32 sWrap, uint32 tWrap) const;
	void SetTextureFormat(int32 width, int32 height, uint32 internalFormat, uint32 format, uint32 type);
	void SetBorderColor(const glm::vec4& color) const;
	void SetSubData(int32 x, int32 y, int32 width, int32 height, const void* data);
	void SetData(const void* data, uint32 size = 0);

	void Bind() const;
	void GenerateMipmap() const;
	
private:
	Texture();
	void CreateTexture();
};