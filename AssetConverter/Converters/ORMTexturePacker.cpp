#include "pch.h"
#include "ORMTexturePacker.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

bool ORMTexturePacker::Convert(const std::string& aoPath, const std::string& roughPath, const std::string& metalPath, const std::string& outPngPath)
{
	RawImage ao, roughness, metallic;

	// 1. 이미지 로드
	bool result = false;
	result = LoadImageToRaw(aoPath, ao);
	result = LoadImageToRaw(roughPath, roughness);
	result = LoadImageToRaw(metalPath, metallic);
	if (!result) return false;

	// 2. 최종 이미지 해상도 결정 (가장 큰 width와 height를 따름)
	int32 finalWidth = 0;
	int32 finalHeight = 0;

	// '확대/축소'를 강제하지 않고, '캔버스 크기'를 가장 큰 입력 맵에 맞춰서 생성
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
	ormImage.channels = 3; // ORM은 RGB 3채널 고정
	ormImage.pixels.resize(finalWidth * finalHeight * 3);

	for (int y = 0; y < finalHeight; ++y)
	{
		for (int x = 0; x < finalWidth; ++x)
		{
			usize idx = (usize)(y * finalWidth + x) * 3;
			uint8 r_val = GetPixelChannel(ao, x, y);
			uint8 g_val = GetPixelChannel(roughness, x, y);
			uint8 b_val = GetPixelChannel(metallic, x, y);

			// ORM 이미지에 값 할당
			ormImage.pixels[idx] = r_val;     // R: AO
			ormImage.pixels[idx + 1] = g_val; // G: Roughness
			ormImage.pixels[idx + 2] = b_val; // B: Metallic
		}
	}

	// 4. 패킹 완료한 이미지를 PNG 파일로 저장 (stb_image_write 사용)
	result = stbi_write_png(
		outPngPath.c_str(),
		ormImage.width,
		ormImage.height,
		ormImage.channels,
		ormImage.pixels.data(),
		ormImage.width * ormImage.channels // stride_in_bytes = width * channels
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

	// 데이터 복사
	usize totalBytes = (usize)width * height * channels;
	outImage.pixels.assign(data, data + totalBytes);

	// stb_image 메모리 해제
	stbi_image_free(data);

	return outImage.IsValid();
}

uint8 ORMTexturePacker::GetPixelChannel(const RawImage& img, int px, int py)
{
	// 유효하지 않거나 범위 밖이면 0 (검은색) 반환
	if (!img.IsValid() || px >= img.width || py >= img.height) return 0;

	// (y * width + x) * channels: 픽셀의 시작 위치
	usize index = (usize)(py * img.width + px) * img.channels;

	// 세 채널 값이 모두 같으므로, 첫 번째 채널(R)을 사용
	return img.pixels[index];
}
