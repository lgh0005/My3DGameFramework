#include "EnginePch.h"
#include "CubeTexture.h"

CubeTexture::CubeTexture()
{
    m_target = GL_TEXTURE_CUBE_MAP;
}
CubeTexture::~CubeTexture() = default;

CubeTexturePtr CubeTexture::Load(const CubeTextureDesc& desc)
{
    std::string actualPath = RESOURCE.ResolvePath(desc.path);

    auto texture = CubeTexturePtr(new CubeTexture());
    texture->m_desc = desc;

    if (!texture->InitFromKtx(actualPath))
        return nullptr;

    return texture;
}

CubeTextureUPtr CubeTexture::Create(int32 width, int32 height, uint32 format, uint32 type)
{
    auto texture = CubeTextureUPtr(new CubeTexture());
    texture->m_desc.name = fmt::format("Procedural_Cube_{}", (uintptr_t)texture.get());
    texture->m_desc.path = "@Virtual/CubeTexture";
    texture->InitProcedural(width, height, format, type);
    return std::move(texture);
}

bool CubeTexture::InitFromKtx(const std::string& path)
{
    ktxTexture2* kTexture = nullptr;
    KTX_error_code result = ktxTexture2_CreateFromNamedFile
    (
        path.c_str(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &kTexture
    );
    if (result != KTX_SUCCESS)
    {
        LOG_ERROR("CubeTexture: KTX2 Load Failed. Path: {}", path);
        return false;
    }

    // 큐브맵인지 확인
    if (!kTexture->isCubemap)
    {
        LOG_ERROR("CubeTexture: Loaded KTX is not a cubemap: {}", path);
        ktxTexture_Destroy(ktxTexture(kTexture));
        return false;
    }

    // BasisU 트랜스코딩
    if (ktxTexture2_NeedsTranscoding(kTexture))
        ktxTexture2_TranscodeBasis(kTexture, KTX_TTF_BC7_RGBA, 0);

    // 부모 헬퍼를 통한 포맷 결정
    m_internalFormat = MapVkFormatToGLInternal(kTexture->vkFormat);

    glGenTextures(1, &m_texture);
    glBindTexture(m_target, m_texture);

    // Immutable Storage 할당 (큐브맵의 경우 depth 대신 6면을 내부적으로 처리)
    glTexStorage2D
    (
        m_target, 
        kTexture->numLevels, 
        m_internalFormat, 
        kTexture->baseWidth, kTexture->baseHeight
    );

    // 6면 데이터 업로드 루프
    UploadKtxData(kTexture);

    m_width = kTexture->baseWidth;
    m_height = kTexture->baseHeight;

    // 기본 파라미터 설정
    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    ktxTexture_Destroy(ktxTexture(kTexture));
    return true;
}

void CubeTexture::UploadKtxData(ktxTexture2* kTexture)
{
    bool isCompressed = kTexture->isCompressed;
    uint32 glType = GetGLDataTypeFromVk(kTexture->vkFormat);
    uint32 pixelFormat = GetPixelFormatFromInternal(m_internalFormat);

    // KTX2 큐브맵 데이터 구조: 각 Level마다 6개의 Face가 연속되어 있음
    for (uint32 level = 0; level < kTexture->numLevels; ++level)
    {
        for (uint32 face = 0; face < 6; ++face)
        {
            ktx_size_t offset;
            ktxTexture_GetImageOffset(ktxTexture(kTexture), level, 0, face, &offset);

            int32 lw = std::max(1, (int32)kTexture->baseWidth >> level);
            int32 lh = std::max(1, (int32)kTexture->baseHeight >> level);
            uint32 size = static_cast<uint32>(ktxTexture_GetImageSize(ktxTexture(kTexture), level));

            // 타겟: GL_TEXTURE_CUBE_MAP_POSITIVE_X (0x8515) ~ NEGATIVE_Z (0x851A)
            GLenum faceTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;

            if (isCompressed)
            {
                glCompressedTexSubImage2D
                (
                    faceTarget, 
                    level, 
                    0, 0, lw, lh,
                    m_internalFormat, 
                    size, 
                    kTexture->pData + offset
                );
            }
            else
            {
                glTexSubImage2D
                (
                    faceTarget, 
                    level, 
                    0, 0, lw, lh, 
                    pixelFormat, 
                    glType, 
                    kTexture->pData + offset
                );
            }
        }
    }
}

void CubeTexture::InitProcedural(int32 width, int32 height, uint32 internalFormat, uint32 type)
{
    glGenTextures(1, &m_texture);
    Bind();

    m_width = width;
    m_height = height;
    m_internalFormat = internalFormat;
    m_type = type;

    // 부모 헬퍼 사용
    m_format = GetPixelFormatFromInternal(m_internalFormat);

    for (uint32 i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_internalFormat,
            m_width, m_height, 0, m_format, m_type, nullptr);
    }

    glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
