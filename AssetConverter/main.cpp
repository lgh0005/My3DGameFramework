#include "pch.h"
#include "Utils/ArgumentParser.h"

int main(int argc, char* argv[])
{
    // Logger 초기화
    MGF3D::LOGGER.Init(argc);

    // 2. 인자 파싱 (ArgumentParser 이용)
    MGF3D::ParseResult args = MGF3D::ArgumentParser::Parse(argc, argv);

    // 3. 파싱 실패 시
    if (!args.IsValid())
    {
        // ArgumentParser 내부에서 이미 에러 로그나 Usage를 출력했을 것임
        // Python 측에 실패(-1)를 알림
        return -1;
    }

    bool success = false;

    // 4. 모드별 분기 실행
    switch (args.mode)
    {
    // [Verify Mode]
    case MGF3D::ConversionMode::Verify:

        // Python GUI가 "--check"를 보냈을 때 "AssetConverter verified" 문자열을 기다림
        std::cout << "AssetConverter verified" << std::endl;
        return 0;

    // [Model Mode]
    case MGF3D::ConversionMode::Model:
        MGF3D::LOG_INFO(">>> [Mode] Model Conversion Selected");
        success = MGF3D::CONV_MODEL.Convert
        (
            args.inputPath, args.outputPath, 
            args.extractOrm, args.flipY
        );
        break;

    // [Animation Mode]
    case MGF3D::ConversionMode::Animation:
        MGF3D::LOG_INFO(">>> [Mode] Animation Conversion Selected");
        success = MGF3D::CONV_ANIM.Convert(args.inputPath, args.refModelPath, args.outputPath);
        break;

    // [ORM Texture Mode]
    case MGF3D::ConversionMode::ORM:
        MGF3D::LOG_INFO(">>> [Mode] ORM Texture Packing Selected");
        success = MGF3D::CONV_ORM.Convert
        (
            args.aoMapPath, args.roughnessMapPath, 
            args.metallicMapPath, args.outputPath,
            args.invertRoughness, args.flipY
        );
        break;

    // [KTX Texture Mode]
    case MGF3D::ConversionMode::KTX:
        MGF3D::LOG_INFO(">>> [Mode] KTX Texture Conversion Selected");
        success = MGF3D::CONV_KTX.Convert
        (
            args.inputPath, args.outputPath, 
            args.ktxFormat, args.ktxColorSpace,
            args.flipY
        );
        break;

    // [KTX CubeMap Mode]
    case MGF3D::ConversionMode::CubeMap:
        MGF3D::LOG_INFO(">>> [Mode] KTX CubeMap Conversion Selected");
        success = MGF3D::CONV_CUBE.Convert(args.cubeFaces, args.outputPath, args.isSRGB);
        break;

    default:
        MGF3D::LOG_ERROR("Unknown conversion mode.");
        success = false;
        break;
    }

    // 5. 결과 리턴
    if (success)
    {
        MGF3D::LOG_INFO(">>> [SUCCESS] Processing finished.");
        return 0;
    }
    else
    {
        MGF3D::LOG_ERROR(">>> [FAILED] Processing failed or not implemented.");
        return 1;
    }
}