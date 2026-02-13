#include "EnginePch.h"
#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Image::Image() = default;
Image::~Image()
{
    if (m_data)  stbi_image_free(m_data);
}

ImagePtr Image::Load(const ImageDesc& desc)
{
    // 1. 확장자 확인
    std::string ext = std::filesystem::path(desc.path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // 2. 확장자에 따른 로딩 방식 분기
    // TODO : 이후에 확장명 종류를 더 구체화해야 한다.
    auto image = ImagePtr(new Image());
    if (ext == ".hdr")
    {
        if (!image->LoadWithStbFloat(desc.path, desc.isFlipY))
            return nullptr;
    }
    else
    {
        if (!image->LoadWithStb(desc.path, desc.isFlipY))
            return nullptr;
    }

    // 3. 리소스 정보 설정
    image->SetName(desc.name);
    image->SetPath(desc.path);

    return image;
}

/*==================================//
//  default image creation methods  //
//==================================*/
ImagePtr Image::Create(int32 width, int32 height, int32 channelCount, int32 bytePerChannel)
{
    auto image = ImagePtr(new Image());
    if (!image->Allocate(width, height, channelCount, bytePerChannel)) return nullptr;
    return image;
}

ImagePtr Image::CreateSingleColorImage(int32 width, int32 height, const glm::vec4& color)
{
    glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f);
    uint8 rgba[4] =
    {
       (uint8)clamped.r, (uint8)clamped.g,
       (uint8)clamped.b, (uint8)clamped.a,
    };
    auto image = Create(width, height, 4);
    for (int i = 0; i < width * height; i++) 
        memcpy(image->m_data + 4 * i, rgba, 4);

    return image;
}

/*==============================//
//  default image load methods  //
//==============================*/
bool Image::LoadWithStb(const std::string& filepath, bool flipVertical)
{
    stbi_set_flip_vertically_on_load(flipVertical);

    m_bytePerChannel = 1;
    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if (!m_data)
    {
        LOG_ERROR("failed to load image: {}", filepath);
        return false;
    }
    return true;
}

bool Image::LoadWithStbFloat(const std::string& filepath, bool flipVertical)
{
    // [주의] 멀티스레드 로딩 시 이 전역 설정 함수는 위험할 수 있습니다.
    // 추후 필요하다면 stbi_load_from_memory 등을 사용하여 직접 처리하거나 락을 걸어야 합니다.
    stbi_set_flip_vertically_on_load(flipVertical);

    m_bytePerChannel = 4;
    m_data = (uint8*)stbi_loadf(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if (!m_data)
    {
        LOG_ERROR("failed to load HDR image: {}", filepath);
        return false;
    }
    return true;
}

bool Image::Allocate(int32 width, int32 height, int32 channelCount, int32 bytePerChannel)
{
    m_width = width;
    m_height = height;
    m_channelCount = channelCount;
    m_bytePerChannel = bytePerChannel;

    usize size = (usize)m_width * m_height * m_channelCount * m_bytePerChannel;
    m_data = (uint8*)malloc(size);

    return m_data ? true : false;
}
