#pragma once
#include "Resources/Textures/TextureBase.h"
#include "Resources/Textures/TextureUtils.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Image)
#pragma endregion

CLASS_PTR(Texture)
class Texture : public TextureBase
{
	friend class TextureUtils;

public:
	virtual ~Texture();
	static const ResourceType s_ResourceType = ResourceType::Texture;
	virtual ResourceType GetResourceType() const override { return ResourceType::Texture; }
	static TextureUPtr Create(int32 width, int32 height, uint32 internalFormat, uint32 format, uint32 type);
	static TextureUPtr CreateMultisample(int32 width, int32 height, int32 samples, uint32 internalFormat);

public:
	void Resize(int32 width, int32 height);
	void SetFilter(uint32 minFilter, uint32 magFilter) const;
	void SetWrap(uint32 sWrap, uint32 tWrap) const;
	void SetTextureFormat(int32 width, int32 height, uint32 internalFormat, uint32 format, uint32 type);
	void SetBorderColor(const glm::vec4& color) const;
	void SetSubData(int32 x, int32 y, int32 width, int32 height, const void* data);
	void SetData(const void* data, uint32 size = 0);

	void Bind() const;
	int32 GetSamples() const { return m_samples; }
	
private:
	Texture();
	void CreateTexture();

	int32   m_samples{ 1 };
};