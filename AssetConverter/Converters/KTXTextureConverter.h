#pragma once

namespace MGF3D
{
	class KTXTextureConverter
	{
		MGF_DECLARE_SINGLE(KTXTextureConverter)

	private:
		KTXTextureConverter();
		~KTXTextureConverter();

	public:
		bool Convert
		(
			const String& inputPath,
			const String& outputPath,
			const String& formatStr,
			const String& colorSpace,
			bool flipY = true
		);

		bool ConvertFromMemory
		(
			uint8* pixels, int32 width, int32 height,
			const String& outputPath,
			const String& formatStr,
			const String& colorSpace,
			bool flipY = false
		);

		/*=================================//
		//   KTXTextureConverter helpers   //
		//=================================*/
	private:

		// LDR (8-bit) 전용
		uint8* LoadLDRImage(const String& path, int32& w, int32& h, int32& ch, bool flipY);
		bool FillMipmapsLDR(ktxTexture2* texture, uint8* basePixels, int32 w, int32 h, uint32 numLevels);

		// HDR (Float) 전용
		float* LoadHDRImage(const String& path, int32& w, int32& h, int32& ch, bool flipY);
		bool FillMipmapsHDR(ktxTexture2* texture, float* basePixels, int32 w, int32 h, uint32 numLevels);

		// 이미지 뒤집기 (thread-safe version)
		void FlipImageVertically(uint8* data, int32 w, int32 h, int32 ch);
		void FlipImageVerticallyFloat(float* data, int32 w, int32 h, int32 ch);

		// 공통 헬퍼
		ktxTexture2* CreateKTX2Object(int32 w, int32 h, uint32 numLevels, uint32 vkFormat);
		bool CompressToBasis(ktxTexture2* texture, const String& formatStr, bool isSRGB);
		bool SaveKTX2ToFile(ktxTexture2* texture, const String& path);
		uint32 MapFormatStringToVk(const String& formatStr, bool isHDR, const String& colorSpace);
	};
}