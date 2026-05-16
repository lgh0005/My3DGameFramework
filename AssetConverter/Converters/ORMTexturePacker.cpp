#include "ConverterPch.h"
#include "ORMTexturePacker.h"

namespace MGF3D
{
	ORMTexturePacker::ORMTexturePacker() = default;
	ORMTexturePacker::~ORMTexturePacker() = default;

	bool ORMTexturePacker::Convert
	(
		const String& aoPath, const String& roughPath,
		const String& metalPath, const String& outPngPath,
		bool invertRoughness, bool flipY
	)
	{
		RawImage combined;
		if (!Pack(aoPath, roughPath, metalPath, combined, invertRoughness, flipY))
			return false;

		int32 result = stbi_write_png
		(
			outPngPath.c_str(),
			combined.width, combined.height, combined.channels,
			combined.pixels.data(), 0
		);

		if (result)
		{
			MGF_LOG_INFO("Successfully saved ORM PNG to: {}", outPngPath);
			return true;
		}

		MGF_LOG_ERROR("Failed to write ORM PNG: {}", outPngPath);
		return false;
	}

	bool ORMTexturePacker::Pack
	(
		const String& aoPath, const String& roughPath,
		const String& metalPath, RawImage& outImage,
		bool invertRoughness, bool flipY
	)
	{
		RawImage ao, roughness, metallic;

		// 1. 이미지 로드
		LoadImageToRaw(aoPath, ao, flipY);
		LoadImageToRaw(roughPath, roughness, flipY);
		LoadImageToRaw(metalPath, metallic, flipY);
		if (!ao.IsValid() && !roughness.IsValid() && !metallic.IsValid())
		{
			MGF_LOG_ERROR("ORM Packing failed: No valid input textures.");
			return false;
		}

		// 2. 최대 해상도 결정
		int32 finalWidth = 0, finalHeight = 0;
		auto UpdateSize = [&](const RawImage& img)
			{
				if (img.IsValid())
				{
					finalWidth = std::max(finalWidth, img.width);
					finalHeight = std::max(finalHeight, img.height);
				}
			};
		UpdateSize(ao); UpdateSize(roughness); UpdateSize(metallic);

		// 3. 출력 이미지 초기화
		outImage.width = finalWidth;
		outImage.height = finalHeight;
		outImage.channels = 4;
		outImage.pixels.resize((usize)finalWidth * finalHeight * 4);

		// 4. 픽셀 합성 루프
		for (int32 y = 0; y < finalHeight; ++y)
		{
			for (int32 x = 0; x < finalWidth; ++x)
			{
				usize idx = (usize)(y * finalWidth + x) * 4;

				// R: AO
				uint8 r_val = 255;
				if (ao.IsValid())
				{
					int32 srcX = (int32)((float)x / finalWidth * ao.width);
					int32 srcY = (int32)((float)y / finalHeight * ao.height);
					r_val = GetPixelChannel(ao, srcX, srcY, 0);
				}

				// G: Roughness
				uint8 g_val = 255;
				if (roughness.IsValid())
				{
					int32 srcX = (int32)((float)x / finalWidth * roughness.width);
					int32 srcY = (int32)((float)y / finalHeight * roughness.height);
					g_val = GetPixelChannel(roughness, srcX, srcY, 1);
					if (invertRoughness) g_val = 255 - g_val;
				}

				// B: Metallic
				uint8 b_val = 255;
				if (metallic.IsValid())
				{
					int32 srcX = (int32)((float)x / finalWidth * metallic.width);
					int32 srcY = (int32)((float)y / finalHeight * metallic.height);
					b_val = GetPixelChannel(metallic, srcX, srcY, 2);
				}

				outImage.pixels[idx] = r_val;
				outImage.pixels[idx + 1] = g_val;
				outImage.pixels[idx + 2] = b_val;
				outImage.pixels[idx + 3] = 255;
			}
		}

		return true;
	}

	bool ORMTexturePacker::LoadImageToRaw
	(
		const String& filepath,
		RawImage& outImage,
		bool flipY
	)
	{
		if (filepath.empty()) return false;

		int32 width, height, channels;
		uint8* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

		if (data == nullptr)
		{
			MGF_LOG_ERROR("Error loading image {} : {}", filepath, stbi_failure_reason());
			return false;
		}

		if (flipY) FlipImageVertically(data, width, height, channels);

		outImage.width = width;
		outImage.height = height;
		outImage.channels = channels;

		// 데이터 복사 및 stb_image 메모리 해제
		usize totalBytes = (usize)width * height * channels;
		outImage.pixels.assign(data, data + totalBytes);
		stbi_image_free(data);

		return outImage.IsValid();
	}

	uint8 ORMTexturePacker::GetPixelChannel(const RawImage& img, int32 px, int32 py, int32 channelOffset)
	{
		// 유효하지 않거나 범위 밖이면 0 (검은색) 반환
		if (!img.IsValid() || px >= img.width || py >= img.height) return 0;

		// (y * width + x) * channels: 픽셀의 시작 위치
		usize index = (usize)(py * img.width + px) * img.channels;

		// 세 채널 값이 모두 같으므로, 첫 번째 채널(R)을 사용
		if (channelOffset >= img.channels) return img.pixels[index];
		return img.pixels[index + channelOffset];
	}

	void ORMTexturePacker::FlipImageVertically(uint8* data, int32 w, int32 h, int32 ch)
	{
		if (!data) return;
		usize rowSize = (usize)w * ch;
		Vector<uint8> tempRow(rowSize);

		for (int32 y = 0; y < h / 2; ++y)
		{
			uint8* top = data + (y * rowSize);
			uint8* bottom = data + ((h - 1 - y) * rowSize);
			memcpy(tempRow.data(), top, rowSize);
			memcpy(top, bottom, rowSize);
			memcpy(bottom, tempRow.data(), rowSize);
		}
	}
}