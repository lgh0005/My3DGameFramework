#include "EnginePch.h"
#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Misc/stb_image.h"

ImageUPtr Image::Load(const std::string& filepath, bool flipVertical)
{
    auto image = ImageUPtr(new Image());
    if (!image->LoadWithStb(filepath, flipVertical)) return nullptr;
    return move(image);
}

ImageUPtr Image::Create(int32 width, int32 height, int32 channelCount)
{
    auto image = ImageUPtr(new Image());
    if (!image->Allocate(width, height, channelCount)) return nullptr;
    return std::move(image);
}

ImageUPtr Image::CreateSingleColorImage(int32 width, int32 height, const glm::vec4& color)
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

    return std::move(image);
}

Image::~Image()
{
    if (m_data) {
        stbi_image_free(m_data);
    }
}

bool Image::LoadWithStb(const std::string& filepath, bool flipVertical)
{
    stbi_set_flip_vertically_on_load(flipVertical);
    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if (!m_data)
    {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }
    return true;
}

bool Image::Allocate(int32 width, int32 height, int32 channelCount)
{
    m_width = width;
    m_height = height;
    m_channelCount = channelCount;
    m_data = (uint8*)malloc(m_width * m_height * m_channelCount);
    return m_data ? true : false;
}
