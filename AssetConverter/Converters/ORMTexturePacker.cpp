#include "pch.h"
#include "ORMTexturePacker.h"
#include <algorithm>
#include <cmath>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

bool ORMTexturePacker::Convert(const std::string& aoPath, const std::string& roughPath, 
	const std::string& metalPath, const std::string& outPngPath, bool invertRoughness)
{
	RawImage ao, roughness, metallic;

	// 1. 이미지 로드 (각각의 로드 실패가 전체 실패로 이어지지 않게 수정)
	LoadImageToRaw(aoPath, ao);
	LoadImageToRaw(roughPath, roughness);
	LoadImageToRaw(metalPath, metallic);
	if (!ao.IsValid() && !roughness.IsValid() && !metallic.IsValid())
	{
		LOG_ERROR("ORM conversion failed: All input paths are invalid or empty.");
		return false;
	}

	// 2. 최종 이미지 해상도 결정 (가장 큰 width와 height를 따름)
	int32 finalWidth = 0;
	int32 finalHeight = 0;

	if (ao.IsValid())
	{
		finalWidth = std::max(finalWidth, ao.width);
		finalHeight = std::max(finalHeight, ao.height);
	}
	if (roughness.IsValid())
	{
		finalWidth = std::max(finalWidth, roughness.width);
		finalHeight = std::max(finalHeight, roughness.height);
	}
	if (metallic.IsValid())
	{
		finalWidth = std::max(finalWidth, metallic.width);
		finalHeight = std::max(finalHeight, metallic.height);
	}

	// 3. 최종 ORM 이미지 생성
	RawImage ormImage;
	ormImage.width = finalWidth;
	ormImage.height = finalHeight;
	ormImage.channels = 3;
	ormImage.pixels.resize((size_t)finalWidth * finalHeight * 3);

	for (int32 y = 0; y < finalHeight; ++y)
	{
		for (int32 x = 0; x < finalWidth; ++x)
		{
			size_t idx = (size_t)(y * finalWidth + x) * 3;

			// 해상도 보정 (Scaling)
			// 현재 픽셀 위치(x, y)를 원본 이미지의 비율에 맞춰 변환합니다.
			// 예: 1024 캔버스의 512 좌표 -> 512 이미지의 256 좌표로 매핑

			// ambient occlusion
			uint8 r_val = 255; // Default AO = 255 (no occlusion)
			if (ao.IsValid()) 
			{
				int32 srcX = (int32)((float)x / finalWidth * ao.width);
				int32 srcY = (int32)((float)y / finalHeight * ao.height);
				r_val = GetPixelChannel(ao, srcX, srcY);
			}

			// roughness
			uint8 g_val = 0; // Default Roughness = 0 (Smooth)
			if (roughness.IsValid()) 
			{
				int32 srcX = (int32)((float)x / finalWidth * roughness.width);
				int32 srcY = (int32)((float)y / finalHeight * roughness.height);
				g_val = GetPixelChannel(roughness, srcX, srcY);
				if (invertRoughness) g_val = (uint8)(255 - g_val); // invert if glossy.
			}

			// metallic
			uint8 b_val = 0; // Default Metallic = 0 (Non-metal)
			if (metallic.IsValid()) 
			{
				int32 srcX = (int32)((float)x / finalWidth * metallic.width);
				int32 srcY = (int32)((float)y / finalHeight * metallic.height);
				b_val = GetPixelChannel(metallic, srcX, srcY);
			}

			// ORM 이미지에 값 할당
			ormImage.pixels[idx] = r_val;     // R: AO
			ormImage.pixels[idx + 1] = g_val; // G: Roughness
			ormImage.pixels[idx + 2] = b_val; // B: Metallic
		}
	}

	// 4. 저장
	int32 result = stbi_write_png
	(
		outPngPath.c_str(),
		ormImage.width,
		ormImage.height,
		ormImage.channels,
		ormImage.pixels.data(),
		0
	);

	if (result)
	{
		LOG_INFO("Successfully saved ORM texture to: {}", outPngPath);
		return true;
	}
	else
	{
		LOG_ERROR("Error writing PNG file: {}", outPngPath);
		return false;
	}
}

bool ORMTexturePacker::LoadImageToRaw(const std::string& filepath, RawImage& outImage)
{
	if (filepath.empty()) return false;

	int32 width, height, channels;
	uint8* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

	if (data == nullptr) 
	{
		LOG_ERROR("Error loading image {} : {}", filepath, stbi_failure_reason());
		return false;
	}

	outImage.width = width;
	outImage.height = height;
	outImage.channels = channels;

	// 데이터 복사 및 stb_image 메모리 해제
	usize totalBytes = (usize)width * height * channels;
	outImage.pixels.assign(data, data + totalBytes);
	stbi_image_free(data);

	return outImage.IsValid();
}

uint8 ORMTexturePacker::GetPixelChannel(const RawImage& img, int32 px, int32 py)
{
	// 유효하지 않거나 범위 밖이면 0 (검은색) 반환
	if (!img.IsValid() || px >= img.width || py >= img.height) return 0;

	// (y * width + x) * channels: 픽셀의 시작 위치
	usize index = (usize)(py * img.width + px) * img.channels;

	// 세 채널 값이 모두 같으므로, 첫 번째 채널(R)을 사용
	return img.pixels[index];
}
