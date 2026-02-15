#pragma once

class KTXTextureConverter
{
	DECLARE_SINGLE(KTXTextureConverter)

public:
	bool Convert
	(
		const std::string& inputPath,
		const std::string& outputPath,
		const std::string& formatStr,
		const std::string& colorSpace,
		bool flipY = true
	);

	bool ConvertFromMemory
	(
		uint8* pixels, int32 width, int32 height,
		const std::string& outputPath, 
		const std::string& formatStr, 
		const std::string& colorSpace,
		bool flipY = false
	);

/*=================================//
//   KTXTextureConverter helpers   //
//=================================*/
private:

	// LDR (8-bit) 전용
	uint8* LoadLDRImage(const std::string& path, int32& w, int32& h, int32& ch, bool flipY);
	bool FillMipmapsLDR(ktxTexture2* texture, uint8* basePixels, int32 w, int32 h, uint32 numLevels);

	// HDR (Float) 전용
	float* LoadHDRImage(const std::string& path, int32& w, int32& h, int32& ch, bool flipY);
	bool FillMipmapsHDR(ktxTexture2* texture, float* basePixels, int32 w, int32 h, uint32 numLevels);

	// 이미지 뒤집기 (thread-safe version)
	void FlipImageVertically(uint8* data, int32 w, int32 h, int32 ch);
	void FlipImageVerticallyFloat(float* data, int32 w, int32 h, int32 ch);

	// 공통 헬퍼
	ktxTexture2* CreateKTX2Object(int32 w, int32 h, uint32 numLevels, uint32 vkFormat);
	bool CompressToBasis(ktxTexture2* texture, const std::string& formatStr, bool isSRGB);
	bool SaveKTX2ToFile(ktxTexture2* texture, const std::string& path);
	uint32 MapFormatStringToVk(const std::string& formatStr, bool isHDR, const std::string& colorSpace);
};