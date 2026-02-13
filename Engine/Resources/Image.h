#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Resource.h"
#include "Resources/ResourceDesc.h"

CLASS_PTR(Image)
class Image : public Resource
{
	DEFINE_RESOURCE_TYPE(ResourceType::Image, ImageDesc)

public:
	virtual ~Image();
	static ImagePtr Load(const ImageDesc& desc);
	static ImagePtr Create(int32 width, int32 height, int32 channelCount = 4, int32 bytePerChannel = 1);
	static ImagePtr CreateSingleColorImage(int32 width,
		int32 height, const glm::vec4& color);

	const uint8* GetData() const	{ return m_data; }
	int32 GetWidth() const			{ return m_width; }
	int32 GetHeight() const			{ return m_height; }
	int32 GetChannelCount() const	{ return m_channelCount; }
	int32 GetBytePerChannel() const { return m_bytePerChannel; }

private:
	Image();
	bool LoadWithStb(const std::string& filepath, bool flipVertical);
	bool LoadWithStbFloat(const std::string& filepath, bool flipVertical);
	bool Allocate(int32 width, int32 height, int32 channelCount, int32 bytePerChannel);
	int32  m_width			{ 0 };
	int32  m_height			{ 0 };
	int32  m_channelCount	{ 0 };
	int32 m_bytePerChannel	{ 1 };
	uint8* m_data			{ nullptr };
};