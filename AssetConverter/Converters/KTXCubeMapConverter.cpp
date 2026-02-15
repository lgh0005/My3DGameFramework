#include "pch.h"
#include "KTXCubeMapConverter.h"

bool KTXCubeMapConverter::Convert
(
	const std::vector<CubeFaceElement>& faceConfigs, 
	const std::string& outputPath,
    bool isSRGB
)
{
    LOG_INFO("KTXCubeMapConverter: Starting CubeMap conversion process...");
    if (faceConfigs.size() != 6)
    {
        LOG_ERROR("KTXCubeMapConverter: Exactly 6 face configurations are required.");
        return false;
    }

    ktxTexture2* texture = nullptr;
    int32 commonWidth = 0, commonHeight = 0;

    // 1. Prepare Texture (사양 확인 및 객체 생성)
    if (!PrepareTexture(faceConfigs, &texture, commonWidth, commonHeight, isSRGB))
        return false;

    // 2. 각 면(Face) 로드 및 데이터 주입
    if (!UploadFaceData(faceConfigs, texture, commonWidth, commonHeight))
    {
        // 실패 시 여기서 해제 (대문자 D 확인)
        ktxTexture_Destroy(ktxTexture(texture));
        return false;
    }

    // 3. 파일 저장 및 정리
    return SaveToFile(texture, outputPath);
}

bool KTXCubeMapConverter::PrepareTexture
(
    const std::vector<CubeFaceElement>& faceConfigs, 
    ktxTexture2** outTexture, 
    int32& outWidth, int32& outHeight,
    bool isSRGB
)
{
    LOG_INFO("KTXCubeMapConverter: [Step 1] Probing specs and creating KTX object...");

    // 첫 번째 이미지로 사양 확인
    stbi_set_flip_vertically_on_load(faceConfigs[0].flipY);
    int w, h, comp;
    unsigned char* testData = stbi_load(faceConfigs[0].path.c_str(), &w, &h, &comp, 4);
    if (!testData)
    {
        LOG_ERROR("KTXCubeMapConverter: Failed to load first face to probe specs: {}", faceConfigs[0].path);
        return false;
    }

    outWidth = w;
    outHeight = h;
    stbi_image_free(testData);

    // KTX2 텍스처 객체 생성 정보 설정
    ktxTextureCreateInfo createInfo = {};
    createInfo.vkFormat = isSRGB ? 43 : 37; // sRGB라면 43(VK_FORMAT_R8G8B8A8_SRGB), 아니면 37(VK_FORMAT_R8G8B8A8_UNORM)
    createInfo.baseWidth = static_cast<uint32_t>(outWidth);
    createInfo.baseHeight = static_cast<uint32_t>(outHeight);
    createInfo.baseDepth = 1;
    createInfo.numDimensions = 2;
    createInfo.numLevels = 1;
    createInfo.numLayers = 1;
    createInfo.numFaces = 6;
    createInfo.isArray = KTX_FALSE;
    createInfo.generateMipmaps = KTX_FALSE;

    auto result = ktxTexture2_Create(&createInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, outTexture);
    if (result != KTX_SUCCESS)
    {
        LOG_ERROR("KTXCubeMapConverter: Failed to create KTX texture object (Error: {})", static_cast<int>(result));
        return false;
    }

    return true;
}

bool KTXCubeMapConverter::UploadFaceData
(
    const std::vector<CubeFaceElement>& faceConfigs, 
    ktxTexture2* texture, 
    int32 width, int32 height
)
{
    LOG_INFO("KTXCubeMapConverter: [Step 2] Uploading 6 face images to storage...");

    for (uint32 i = 0; i < 6; ++i)
    {
        stbi_set_flip_vertically_on_load(faceConfigs[i].flipY);

        int32 w, h, comp;
        unsigned char* data = stbi_load(faceConfigs[i].path.c_str(), &w, &h, &comp, 4);

        if (!data)
        {
            LOG_ERROR("KTXCubeMapConverter: Failed to load face {}: {}", i, faceConfigs[i].path);
            return false;
        }

        if (w != width || h != height)
        {
            LOG_ERROR
            (
                "KTXCubeMapConverter: Dimension mismatch at face {}. Expected {}x{}, got {}x{}", 
                i, width, height, w, h
            );
            stbi_image_free(data);
            return false;
        }

        if (faceConfigs[i].flipX)
            FlipImageHorizontal(data, w, h, 4);

        ktx_size_t imageSize = static_cast<ktx_size_t>(w) * h * 4;
        auto result = ktxTexture_SetImageFromMemory(ktxTexture(texture), 0, 0, i, data, imageSize);
        stbi_image_free(data);

        if (result != KTX_SUCCESS)
        {
            LOG_ERROR("KTXCubeMapConverter: Failed to set image for face {}. (Error: {})", i, (int)result);
            return false;
        }
    }

    return true;
}

void KTXCubeMapConverter::FlipImageHorizontal(unsigned char* data, int32 width, int32 height, int32 channels)
{
    int32 rowSize = width * channels;
    for (int32 y = 0; y < height; ++y)
    {
        unsigned char* row = data + (y * rowSize);
        for (int32 x = 0; x < width / 2; ++x)
        {
            int32 leftIdx = x * channels;
            int32 rightIdx = (width - 1 - x) * channels;
            for (int32 c = 0; c < channels; ++c)
                std::swap(row[leftIdx + c], row[rightIdx + c]);
        }
    }
}

bool KTXCubeMapConverter::SaveToFile(ktxTexture2* texture, const std::string& outputPath)
{
    LOG_INFO("KTXCubeMapConverter: [Step 3] Writing to file: {}", outputPath);

    auto result = ktxTexture_WriteToNamedFile(ktxTexture(texture), outputPath.c_str());
    ktxTexture_Destroy(ktxTexture(texture));

    if (result != KTX_SUCCESS)
    {
        LOG_ERROR("KTXCubeMapConverter: Failed to write file: {} (Error: {})", outputPath, (int)result);
        return false;
    }

    LOG_INFO("KTXCubeMapConverter: Successfully generated cubemap -> {}", outputPath);
    return true;
}
