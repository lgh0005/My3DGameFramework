#pragma once

CLASS_PTR(Image)
class Image
{
public:
	static ImageUPtr Load(const std::string& filepath);
	~Image();

	const uint8* GetData() const	{ return m_data; }
	int32 GetWidth() const			{ return m_width; }
	int32 GetHeight() const			{ return m_height; }
	int32 GetChannelCount() const	{ return m_channelCount; }

private:
	Image() = default;
	bool LoadWithStb(const std::string& filepath);
	int m_width			{ 0 };
	int m_height		{ 0 };
	int m_channelCount	{ 0 };
	uint8_t* m_data		{ nullptr };
};