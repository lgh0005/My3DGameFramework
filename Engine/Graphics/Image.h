#pragma once

CLASS_PTR(Image)
class Image
{
public:
	static ImageUPtr Load(const std::string& filepath, bool flipVertical = true);
	static ImageUPtr Create(int32 width, int32 height, int32 channelCount = 4);
	static ImageUPtr CreateSingleColorImage(int32 width,
		int32 height, const glm::vec4& color);
	~Image();

	const uint8* GetData() const	{ return m_data; }
	int32 GetWidth() const			{ return m_width; }
	int32 GetHeight() const			{ return m_height; }
	int32 GetChannelCount() const	{ return m_channelCount; }

private:
	Image() = default;
	bool LoadWithStb(const std::string& filepath, bool flipVertical);
	bool Allocate(int32 width, int32 height, int32 channelCount);
	int32  m_width			{ 0 };
	int32  m_height			{ 0 };
	int32  m_channelCount	{ 0 };
	uint8* m_data			{ nullptr };
};