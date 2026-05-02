#pragma once

namespace MGF3D
{
    enum class ConversionMode
    {
        None = 0,
        Verify,     // --check (Python 연동 확인용)     
        Model,      // -m
        Animation,  // -a
        ORM,        // --orm
        KTX,        // --ktx
        CubeMap     // --cubemap
    };

    struct ParseResult
    {
        ConversionMode mode = ConversionMode::None;

        // [공통] 출력 경로 (파일 전체 경로)
        String outputPath;
        bool flipY = true;

        // [Model]
        String inputPath;
        bool extractOrm = false;

        // [Animation] 애니메이션의 기준이 되는 모델 파일
        String refModelPath;

        // [ORM Texture] 개별 PBR 텍스처 패킹용
        String aoMapPath;
        String roughnessMapPath;
        String metallicMapPath;
        bool invertRoughness = false;

        // [KTX Texture]
        String ktxFormat = "BC7";
        String ktxColorSpace = "sRGB";

        // [CubeMap]
        Vector<CubeFaceElement> cubeFaces;
        bool isSRGB = true;

        bool IsValid() const;
    };
}