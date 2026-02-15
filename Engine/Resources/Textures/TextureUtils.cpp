#include "EnginePch.h"
#include "TextureUtils.h"
#include "Resources/Textures/Texture.h"
#include "Resources/Textures/CubeTexture.h"
#include "Resources/Image.h"
#include <ktx.h>
#include <stb_image.h>

/*===========================//
//   Static Member Init      //
//===========================*/
TexturePtr TextureUtils::s_whiteTex = nullptr;
TexturePtr TextureUtils::s_grayTex = nullptr;
TexturePtr TextureUtils::s_blackTex = nullptr;
TexturePtr TextureUtils::s_blueTex = nullptr;

/*===========================//
//   Texture Loading Utils   //
//===========================*/
TextureUPtr TextureUtils::LoadTexture(const std::string& path, bool sRGB)
{
    // 확장자 추출 및 소문자 변환
    std::string ext = std::filesystem::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    // 2. KTX 파일 처리
    if (ext == ".ktx")
    {
        return LoadTextureFromKtx(path);
    }

    // 3. 이미지 로드
    ImageDesc imgDesc(path);
    imgDesc.isFlipY = true;

    auto image = Image::Load(imgDesc);
    if (!image) return nullptr;

    // 4. 로드된 데이터 타입에 따라 분기 처리
    if (image->GetBytePerChannel() == 4) return LoadTextureFromHDR(image.get());
    else return LoadTextureFromImage(image.get(), sRGB);
}

TextureUPtr TextureUtils::LoadTextureFromImage(const Image* image, bool sRGB)
{
    // 1. 빈 텍스처 생성
    auto texture = TextureUPtr(new Texture());

    // 2. ID 생성 (Texture의 private 함수 호출 가능)
    texture->CreateTexture();

    // 3. 포맷 결정
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount())
    {
    case 1: format = GL_RED; break;
    case 2: format = GL_RG; break;
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
    }

    // 4. Texture 멤버 변수 직접 설정 (friend 권한)
    texture->m_width = image->GetWidth();
    texture->m_height = image->GetHeight();
    texture->m_format = format;
    texture->m_type = GL_UNSIGNED_BYTE;

    // Internal Format 결정 (RGB -> RGB8, RGBA -> RGBA8)
    if (format == GL_RGBA) texture->m_internalFormat = GL_RGBA8;
    else if (format == GL_RGB) texture->m_internalFormat = GL_RGB8;
    else texture->m_internalFormat = format;

    // 5. 데이터 업로드
    texture->Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, texture->m_internalFormat,
        texture->m_width, texture->m_height, 0,
        format, texture->m_type, image->GetData());

    // 6. 밉맵 생성
    glGenerateMipmap(GL_TEXTURE_2D);

    return std::move(texture);
}

TextureUPtr TextureUtils::LoadTextureFromHDR(const Image* image)
{
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();

    GLenum internalFormat = GL_RGB16F;
    GLenum format = GL_RGB;

    // 채널 수에 따른 포맷 결정
    switch (image->GetChannelCount())
    {
    case 3: internalFormat = GL_RGBA16F; format = GL_RGB; break;
    case 4: internalFormat = GL_RGBA16F; format = GL_RGBA; break;
    }

    // HDR 데이터 업로드
    texture->SetTextureFormat(image->GetWidth(), image->GetHeight(), internalFormat, format, GL_FLOAT);
    texture->SetData(image->GetData());

    // HDR 전용 파라미터
    texture->SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);

    return std::move(texture);
}

TextureUPtr TextureUtils::LoadTextureFromKtx(const std::string& path)
{
    auto texture = LoadKtxInternal(path);
    if (!texture) return nullptr;

    // 2D 텍스처 기본 설정
    texture->SetWrap(GL_REPEAT, GL_REPEAT);
    texture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    return std::move(texture);
}

/*================================//
//   Cube Texture Loading Utils   //
//================================*/
CubeTextureUPtr TextureUtils::LoadCubeMapFromImages(const std::vector<Image*> images)
{
    // 1. 빈 큐브 텍스처 생성
    auto texture = CubeTextureUPtr(new CubeTexture());
    
    // 2. ID 생성 및 바인딩 (직접 GL 호출)
    glGenTextures(1, &texture->m_texture);
    texture->Bind();

    // 3. 파라미터 설정
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // 4. 포맷 결정 (첫 번째 이미지 기준)
    GLenum internalFormat, format, type;
    GetFormatsFromImage(images[0], internalFormat, format, type);

    // 5. 멤버 변수 설정
    texture->m_width = images[0]->GetWidth();
    texture->m_height = images[0]->GetHeight();
    texture->m_type = type;
    texture->m_format = internalFormat;

    // 6. 6면 데이터 업로드
    for (uint32 i = 0; i < 6; i++)
    {
        if (!images[i]) return nullptr;

        glTexImage2D
        (
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0,
            internalFormat,
            texture->m_width, texture->m_height,
            0,
            format, // Upload Format
            type,
            images[i]->GetData()
        );
    }

    return std::move(texture);
}

CubeTextureUPtr TextureUtils::LoadCubeMapFromKtx(const std::string& path)
{
    return LoadKtxCubeInternal(path);
}

/*===========================//
//   Default Texture Utils   //
//===========================*/
TexturePtr TextureUtils::GetWhiteTexture()
{
    if (s_whiteTex) return s_whiteTex;
    std::vector<uint8> data = { 255, 255, 255, 255 };
    s_whiteTex = Create4x4Texture(data);
    return s_whiteTex;
}

TexturePtr TextureUtils::GetGrayTexture()
{
    if (s_grayTex) return s_grayTex;
    std::vector<uint8> data = { 128, 128, 128, 255 };
    s_grayTex = Create4x4Texture(data);
    return s_grayTex;
}

TexturePtr TextureUtils::GetBlackTexture()
{
    if (s_blackTex) return s_blackTex;
    std::vector<uint8> data = { 0, 0, 0, 255 };
    s_blackTex = Create4x4Texture(data);
    return s_blackTex;
}

TexturePtr TextureUtils::GetBlueTexture()
{
    if (s_blueTex) return s_blueTex;
    std::vector<uint8> data = { 128, 128, 255, 255 };
    s_blueTex = Create4x4Texture(data);
    return s_blueTex;
}

/*======================//
//   Internal Helpers   //
//======================*/
TexturePtr TextureUtils::Create4x4Texture(const std::vector<uint8>& colorData)
{
    auto texture = Texture::Create(4, 4, GL_RGBA16F, GL_RGBA, GL_UNSIGNED_BYTE);

    std::vector<uint8> fullBuffer;
    fullBuffer.reserve(4 * 4 * 4);

    for (int i = 0; i < 16; ++i)
        fullBuffer.insert(fullBuffer.end(), colorData.begin(), colorData.end());

    texture->SetData(fullBuffer.data(), (uint32)fullBuffer.size());
    return std::move(texture);
}

TextureUPtr TextureUtils::LoadKtxInternal(const std::string& ktxFilePath)
{
    ktxTexture2* kTexture = nullptr; // KTX2 포맷 사용 (BasisU 대응)
    KTX_error_code result = ktxTexture2_CreateFromNamedFile(
        ktxFilePath.c_str(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &kTexture
    );

    if (result != KTX_SUCCESS)
    {
        LOG_ERROR("TextureUtils: KTX2 Create failed ({}): {}", ktxErrorString(result), ktxFilePath);
        return nullptr;
    }

    // 1. 트랜스코딩 (Basis Universal 압축인 경우)
    if (ktxTexture2_NeedsTranscoding(kTexture))
    {
        // 데스크톱 OpenGL 환경에서 가장 품질이 좋은 BC7 포맷으로 변환하도록 지시
        result = ktxTexture2_TranscodeBasis(kTexture, KTX_TTF_BC7_RGBA, 0);
        if (result != KTX_SUCCESS)
        {
            LOG_ERROR("TextureUtils: Transcoding failed: {}", ktxFilePath);
            ktxTexture_Destroy(ktxTexture(kTexture));
            return nullptr;
        }
    }

    // 2. 포맷 매핑 (vkFormat -> OpenGL)
    GLenum internalFormat = GL_RGBA8;
    bool isCompressed = kTexture->isCompressed;
    GLenum type = GL_UNSIGNED_BYTE;

    switch (kTexture->vkFormat)
    {
    case 146: internalFormat = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM; break; // BC7 sRGB
    case 145: internalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM; break;      // BC7 Linear
    case 97:  internalFormat = GL_RGBA16F; type = GL_HALF_FLOAT; isCompressed = false; break; // HDR
    case 43:  internalFormat = GL_SRGB8_ALPHA8; isCompressed = false; break;
    case 37:  internalFormat = GL_RGBA8; isCompressed = false; break;
    default:
        LOG_WARN("TextureUtils: Unknown vkFormat {}, falling back to RGBA8", kTexture->vkFormat);
        break;
    }

    // 3. 엔진 Texture 객체 생성
    auto texture = TextureUPtr(new Texture());
    glGenTextures(1, &texture->m_texture);
    texture->m_width = kTexture->baseWidth;
    texture->m_height = kTexture->baseHeight;
    texture->m_target = GL_TEXTURE_2D;
    texture->m_internalFormat = internalFormat;

    // 4. Immutable Storage 할당
    texture->Bind();
    glTexStorage2D(GL_TEXTURE_2D, kTexture->numLevels, internalFormat, texture->m_width, texture->m_height);

    // 5. 각 밉맵 레벨 데이터 업로드
    for (uint32 level = 0; level < kTexture->numLevels; ++level)
    {
        ktx_size_t offset;
        ktxTexture_GetImageOffset(ktxTexture(kTexture), level, 0, 0, &offset);
        uint8* data = kTexture->pData + offset;
        uint32 imageSize = static_cast<uint32>(ktxTexture_GetImageSize(ktxTexture(kTexture), level));

        int32 lw = std::max(1, (int)texture->m_width >> level);
        int32 lh = std::max(1, (int)texture->m_height >> level);

        if (isCompressed)
        {
            glCompressedTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, lw, lh, internalFormat, imageSize, data);
        }
        else
        {
            // HDR(GL_HALF_FLOAT) 혹은 비압축 데이터 처리
            glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, lw, lh, GL_RGBA, type, data);
        }
    }

    ktxTexture_Destroy(ktxTexture(kTexture));
    LOG_INFO("Texture loaded from KTX (Immutable): {} ({}x{})", ktxFilePath, texture->m_width, texture->m_height);

    return std::move(texture);
}

//TextureUPtr TextureUtils::LoadKtxInternal(const std::string& ktxFilePath)
//{
//    ktxTexture* kTexture;
//    KTX_error_code result;
//    GLuint textureID = 0;
//    GLenum target;
//    GLenum glerror;
//
//    // 1. KTX 로드
//    result = ktxTexture_CreateFromNamedFile(
//        ktxFilePath.c_str(),
//        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
//        &kTexture
//    );
//
//    if (result != KTX_SUCCESS)
//    {
//        LOG_ERROR("TextureUtils: Failed to load KTX texture: {}", ktxFilePath);
//        return nullptr;
//    }
//
//    // 2. GPU 업로드
//    result = ktxTexture_GLUpload(kTexture, &textureID, &target, &glerror);
//    if (result != KTX_SUCCESS)
//    {
//        ktxTexture_Destroy(kTexture);
//        LOG_ERROR("TextureUtils: Failed to upload KTX texture: {}", ktxFilePath);
//        return nullptr;
//    }
//
//    // 3. Texture 객체 생성 및 멤버 주입 (Friend 접근)
//    auto texture = TextureUPtr(new Texture());
//    texture->m_texture = textureID;
//    texture->m_width = kTexture->baseWidth;
//    texture->m_height = kTexture->baseHeight;
//    texture->m_target = target;
//
//    // 포맷 쿼리
//    GLint format = 0;
//    glBindTexture(target, textureID);
//    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
//    texture->m_format = format;
//
//    ktxTexture_Destroy(kTexture);
//    return std::move(texture);
//}

CubeTextureUPtr TextureUtils::LoadKtxCubeInternal(const std::string& ktxFilePath)
{
    ktxTexture* kTexture;
    KTX_error_code result;
    GLuint textureID = 0;
    GLenum target;
    GLenum glerror;

    // 1. KTX 로드
    result = ktxTexture_CreateFromNamedFile(
        ktxFilePath.c_str(),
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &kTexture
    );

    if (result != KTX_SUCCESS)
    {
        LOG_ERROR("TextureUtils: Failed to load KTX cubemap: {}", ktxFilePath);
        return nullptr;
    }

    if (!kTexture->isCubemap)
    {
        LOG_ERROR("TextureUtils: Texture is not a cubemap: {}", ktxFilePath);
        ktxTexture_Destroy(kTexture);
        return nullptr;
    }

    // 2. GPU 업로드
    result = ktxTexture_GLUpload(kTexture, &textureID, &target, &glerror);
    if (result != KTX_SUCCESS)
    {
        LOG_ERROR("TextureUtils: Failed to upload KTX cubemap: {}", ktxFilePath);
        ktxTexture_Destroy(kTexture);
        return nullptr;
    }

    // 3. CubeTexture 생성
    auto texture = CubeTextureUPtr(new CubeTexture());
    texture->m_texture = textureID;
    texture->m_width = kTexture->baseWidth;
    texture->m_height = kTexture->baseHeight;

    // 포맷 쿼리
    GLint format = 0;
    glBindTexture(target, textureID);
    glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
    texture->m_format = format;
    texture->m_type = GL_UNSIGNED_BYTE;

    // 4. 파라미터 설정
    texture->Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    ktxTexture_Destroy(kTexture);
    return std::move(texture);
}

void TextureUtils::GetFormatsFromImage(const Image* image, GLenum& outInternal, GLenum& outFormat, GLenum& outType)
{
    // HDR 여부 확인
    bool isHDR = (image->GetBytePerChannel() == 4);
    outType = isHDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

    // 기본 포맷 결정
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount())
    {
    case 1: format = GL_RED; break;
    case 2: format = GL_RG; break;
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
    }
    outFormat = format;

    // 내부 포맷 결정
    if (isHDR)
    {
        if (format == GL_RGB) outInternal = GL_RGB16F;
        else if (format == GL_RGBA) outInternal = GL_RGBA16F;
        else if (format == GL_RED) outInternal = GL_R16F;
        else outInternal = format;
    }
    else
    {
        outInternal = format; // LDR은 기본 포맷 사용
    }
}