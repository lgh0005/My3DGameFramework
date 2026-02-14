#pragma once
#include "Resources/ResourceDesc.h"
#include "Resources/Resource.h"

// TODO : 이후에는 ktx로만 로딩을 할 것이기에 이 클래스는 필요가 없어질 수도 있다.
// 또는 레거시로 그냥 두는 클래스로 변할 가능성이 높음.
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
	virtual const ImageDesc& GetDesc() const override { return m_desc; }

	const uint8* GetData() const	{ return m_data; }
	int32 GetWidth() const			{ return m_width; }
	int32 GetHeight() const			{ return m_height; }
	int32 GetChannelCount() const	{ return m_channelCount; }
	int32 GetBytePerChannel() const { return m_bytePerChannel; }

private:
	Image();
	bool LoadWithStb();
	bool LoadWithStbFloat();
	bool Allocate(int32 width, int32 height, int32 channelCount, int32 bytePerChannel);
	
	ImageDesc m_desc;
	int32  m_width			{ 0 };
	int32  m_height			{ 0 };
	int32  m_channelCount	{ 0 };
	int32 m_bytePerChannel	{ 1 };
	uint8* m_data			{ nullptr };
};