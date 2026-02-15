#include "pch.h"
#include "KTXTextureConverter.h"

bool KTXTextureConverter::Convert
(
	const std::string& inputPath,
	const std::string& outputPath,
	const std::string& formatStr,
	const std::string& colorSpace
)
{
	int32 width, height, channels;
	bool isHDR = stbi_is_hdr(inputPath.c_str());
	uint32 vkFormat = MapFormatStringToVk(formatStr, isHDR, colorSpace);

	bool success = false;
	ktxTexture2* texture = nullptr;
	uint32 numLevels = 0;

	// HDR 처리 경로
	if (isHDR)
	{
		float* basePixels = LoadHDRImage(inputPath, width, height, channels);
		if (!basePixels) return false;

		numLevels = static_cast<uint32>(floor(log2(std::max(width, height)))) + 1;
		texture = CreateKTX2Object(width, height, numLevels, vkFormat);
		if (texture) success = FillMipmapsHDR(texture, basePixels, width, height, numLevels);
		stbi_image_free(basePixels);
	}

	// LDR 처리 경로
	else
	{
		uint8* basePixels = LoadLDRImage(inputPath, width, height, channels);
		if (!basePixels) return false;

		numLevels = static_cast<uint32>(floor(log2(std::max(width, height)))) + 1;
		uint32 initialFormat = (colorSpace == "sRGB") ? 43 : 37;
		texture = CreateKTX2Object(width, height, numLevels, initialFormat);

		if (texture)
		{
			if (FillMipmapsLDR(texture, basePixels, width, height, numLevels))
			{
				if (vkFormat != 43 && vkFormat != 37)
					success = CompressToBasis(texture, formatStr, (colorSpace == "sRGB"));
				else
					success = true;
			}
		}
		stbi_image_free(basePixels);
	}

	// 저장 및 정리
	if (success && texture)
		success = SaveKTX2ToFile(texture, outputPath);

	if (texture) ktxTexture_Destroy(ktxTexture(texture));

	if (success) LOG_INFO("Successfully converted -> {}", outputPath);
	return success;
}

bool KTXTextureConverter::ConvertFromMemory
(
	uint8* pixels, int32 width, int32 height, 
	const std::string& outputPath, 
	const std::string& formatStr, 
	const std::string& colorSpace
)
{
	if (!pixels) return false;

	// LDR(8-bit) 변환이므로 isHDR은 항상 false
	uint32 vkFormat = MapFormatStringToVk(formatStr, false, colorSpace);
	uint32 numLevels = static_cast<uint32>(floor(log2(std::max(width, height)))) + 1;

	// 8비트 초기 컨테이너 포맷 결정
	uint32 initialFormat = (colorSpace == "sRGB") ? 43 : 37;
	ktxTexture2* texture = CreateKTX2Object(width, height, numLevels, initialFormat);
	if (!texture) return false;

	bool success = false;

	// 밉맵 생성 및 데이터 주입
	if (FillMipmapsLDR(texture, pixels, width, height, numLevels))
	{
		// 압축이 필요한 경우 Basis 압축 진행
		if (vkFormat != 43 && vkFormat != 37)
			success = CompressToBasis(texture, formatStr, (colorSpace == "sRGB"));
		else
			success = true;
	}

	// 파일 저장
	if (success)
	{
		success = SaveKTX2ToFile(texture, outputPath);
		if (success) LOG_INFO("Successfully converted from memory -> {}", outputPath);
	}

	ktxTexture_Destroy(ktxTexture(texture));
	return success;
}

/*=================================//
//          LDR Helpers            //
//=================================*/
uint8* KTXTextureConverter::LoadLDRImage(const std::string& path, int32& w, int32& h, int32& ch)
{
	uint8* pixels = stbi_load(path.c_str(), &w, &h, &ch, STBI_rgb_alpha);
	if (!pixels) LOG_ERROR("LDR Load Failed: {}", path);
	return pixels;
}

bool KTXTextureConverter::FillMipmapsLDR(ktxTexture2* texture, uint8* basePixels, int32 w, int32 h, uint32 numLevels)
{
	uint8* currentPixels = basePixels;
	int32 currentW = w;
	int32 currentH = h;

	for (uint32 level = 0; level < numLevels; ++level)
	{
		ktx_size_t levelSize = static_cast<ktx_size_t>(currentW) * currentH * 4;
		ktx_error_code_e res = ktxTexture_SetImageFromMemory(ktxTexture(texture), level, 0, 0, currentPixels, levelSize);

		if (res != KTX_SUCCESS)
		{
			LOG_ERROR("KTXConverter: SetImageFromMemory failed at LDR level {} (Error: {})", level, (int32)res);
			if (currentPixels != basePixels) free(currentPixels);
			return false;
		}

		if (level < numLevels - 1)
		{
			int32 nextW = std::max(1, currentW / 2);
			int32 nextH = std::max(1, currentH / 2);
			uint8* nextPixels = static_cast<uint8*>(malloc(static_cast<usize>(nextW) * nextH * 4));
			stbir_resize_uint8_linear(currentPixels, currentW, currentH, 0, nextPixels, nextW, nextH, 0, STBIR_RGBA);

			if (currentPixels != basePixels) free(currentPixels);
			currentPixels = nextPixels;
			currentW = nextW;
			currentH = nextH;
		}
	}
	if (currentPixels != basePixels) free(currentPixels);
	return true;
}

/*=================================//
//          HDR Helpers            //
//=================================*/
float* KTXTextureConverter::LoadHDRImage(const std::string& path, int32& w, int32& h, int32& ch)
{
	float* pixels = stbi_loadf(path.c_str(), &w, &h, &ch, STBI_rgb_alpha);
	if (!pixels) LOG_ERROR("HDR Load Failed: {}", path);
	return pixels;
}

bool KTXTextureConverter::FillMipmapsHDR(ktxTexture2* texture, float* basePixels, int32 w, int32 h, uint32 numLevels)
{
	float* currentPixels = basePixels;
	int32 currentW = w;
	int32 currentH = h;
	uint32 targetFormat = texture->vkFormat;

	for (uint32 level = 0; level < numLevels; ++level)
	{
		const void* dataToSet = nullptr;
		ktx_size_t levelSize = 0;
		std::vector<uint16> halfBuffer;

		// 포맷이 RGBA16F(97)라면 32비트 데이터를 16비트로 변환
		if (targetFormat == 97)
		{
			usize numElements = static_cast<size_t>(currentW) * currentH * 4;
			halfBuffer.resize(numElements);

			for (usize i = 0; i < numElements; ++i)
				halfBuffer[i] = Utils::FloatToHalf(currentPixels[i]);

			dataToSet = halfBuffer.data();
			levelSize = numElements * sizeof(uint16);
		}
		else
		{
			dataToSet = currentPixels;
			levelSize = static_cast<ktx_size_t>(currentW) * currentH * 4 * sizeof(float);
		}

		// 주입
		ktx_error_code_e res = ktxTexture_SetImageFromMemory
		(
			ktxTexture(texture),
			level, 0, 0,
			reinterpret_cast<const ktx_uint8_t*>(dataToSet),
			levelSize
		);

		if (res != KTX_SUCCESS)
		{
			LOG_ERROR("KTXConverter: SetImageFromMemory failed at HDR level {} (Error: {})", level, (int32)res);
			if (currentPixels != basePixels) free(currentPixels);
			return false;
		}

		// 다음 밉맵 생성 (항상 원본 정밀도인 float32에서 리사이징을 수행해야 품질이 유지됩니다)
		if (level < numLevels - 1)
		{
			int32 nextW = std::max(1, currentW / 2);
			int32 nextH = std::max(1, currentH / 2);
			float* nextPixels = static_cast<float*>(malloc(static_cast<usize>(nextW) * nextH * 4 * sizeof(float)));

			stbir_resize_float_linear(currentPixels, currentW, currentH, 0, nextPixels, nextW, nextH, 0, STBIR_RGBA);

			if (currentPixels != basePixels) free(currentPixels);
			currentPixels = nextPixels;
			currentW = nextW;
			currentH = nextH;
		}
	}

	if (currentPixels != basePixels) free(currentPixels);
	return true;
}
/*=================================//
//          Common Helpers         //
//=================================*/
ktxTexture2* KTXTextureConverter::CreateKTX2Object(int32 w, int32 h, uint32 numLevels, uint32 vkFormat)
{
	ktxTexture2* texture = nullptr;
	ktxTextureCreateInfo createInfo = {};
	createInfo.vkFormat = vkFormat;
	createInfo.baseWidth = static_cast<uint32>(w);
	createInfo.baseHeight = static_cast<uint32>(h);
	createInfo.baseDepth = 1;
	createInfo.numDimensions = 2;
	createInfo.numLevels = numLevels;
	createInfo.numLayers = 1;
	createInfo.numFaces = 1;

	if (ktxTexture2_Create(&createInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, &texture) != KTX_SUCCESS)
	{
		LOG_ERROR("KTX2 Creation Failed (Format: {})", vkFormat);
		return nullptr;
	}
	return texture;
}

bool KTXTextureConverter::CompressToBasis(ktxTexture2* texture, const std::string& formatStr, bool isSRGB)
{
	LOG_INFO("Compressing LDR to {} ({})...", formatStr, isSRGB ? "sRGB" : "Linear");
	ktxBasisParams params = {};
	params.structSize = sizeof(params);
	params.uastc = KTX_TRUE;
	params.compressionLevel = 2;
	return ktxTexture2_CompressBasisEx(texture, &params) == KTX_SUCCESS;
}

bool KTXTextureConverter::SaveKTX2ToFile(ktxTexture2* texture, const std::string& path)
{
	return ktxTexture_WriteToNamedFile(ktxTexture(texture), path.c_str()) == KTX_SUCCESS;
}

uint32 KTXTextureConverter::MapFormatStringToVk(const std::string& formatStr, bool isHDR, const std::string& colorSpace)
{
	if (isHDR) return 97; // VK_FORMAT_R16G16B16A16_SFLOAT
	bool isSRGB = (colorSpace == "sRGB");

	// LDR용 포맷 매핑
	if (formatStr == "BC1")   return isSRGB ? 133 : 131;
	if (formatStr == "BC3")   return isSRGB ? 139 : 137;
	if (formatStr == "BC7")   return isSRGB ? 146 : 145;
	if (formatStr == "RGBA8") return isSRGB ? 43 : 37;
	return isSRGB ? 43 : 37;
}
