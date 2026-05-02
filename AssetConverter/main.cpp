#include "ConverterPch.h"
#include "Core/ParseResult.h"
#include "Core/ArgumentParser.h"
#include "Managers/ThreadManager.h"
#include "Converters/ModelConverter.h"
#include "Converters/AnimationConverter.h"
#include "Converters/KTXCubeMapConverter.h"
#include "Converters/KTXTextureConverter.h"
#include "Converters/ORMTexturePacker.h"

int main(int argc, char* argv[])
{
    // Logger 초기화
    MGF_LOG_INIT();

    // 2. 인자 파싱 (ArgumentParser 이용)
    MGF3D::ParseResult args = MGF3D::ArgumentParser::Parse(argc, argv);

    // 3. 파싱 실패 시
    if (!args.IsValid())
    {
        // ArgumentParser 내부에서 이미 에러 로그나 Usage를 출력했을 것임
        // Python 측에 실패(-1)를 알림
        return -1;
    }

    // 4. 워커 생성
    MGF_THREAD.Init(std::thread::hardware_concurrency());

    // 5. 모드별 분기 실행
    bool success = false;
    switch (args.mode)
    {
    // [Verify Mode]
    case MGF3D::ConversionMode::Verify:

        // Python GUI가 "--check"를 보냈을 때 "AssetConverter verified" 문자열을 기다림
        std::cout << "AssetConverter verified" << std::endl;
        return 0;

    // [Model Mode]
    case MGF3D::ConversionMode::Model:
        MGF_LOG_INFO(">>> [Mode] Model Conversion Selected");
        success = MGF_CONV_MODEL.Convert
        (
            args.inputPath, args.outputPath, 
            args.extractOrm, args.flipY
        );
        break;

    // [Animation Mode]
    case MGF3D::ConversionMode::Animation:
        MGF_LOG_INFO(">>> [Mode] Animation Conversion Selected");
        success = MGF_CONV_ANIM.Convert(args.inputPath, args.refModelPath, args.outputPath);
        break;

    // [ORM Texture Mode]
    case MGF3D::ConversionMode::ORM:
        MGF_LOG_INFO(">>> [Mode] ORM Texture Packing Selected");
        success = MGF_CONV_ORM.Convert
        (
            args.aoMapPath, args.roughnessMapPath, 
            args.metallicMapPath, args.outputPath,
            args.invertRoughness, args.flipY
        );
        break;

    // [KTX Texture Mode]
    case MGF3D::ConversionMode::KTX:
        MGF_LOG_INFO(">>> [Mode] KTX Texture Conversion Selected");
        success = MGF_CONV_KTX.Convert
        (
            args.inputPath, args.outputPath, 
            args.ktxFormat, args.ktxColorSpace,
            args.flipY
        );
        break;

    // [KTX CubeMap Mode]
    case MGF3D::ConversionMode::CubeMap:
        MGF_LOG_INFO(">>> [Mode] KTX CubeMap Conversion Selected");
        success = MGF_CONV_CUBE.Convert(args.cubeFaces, args.outputPath, args.isSRGB);
        break;

    default:
        MGF_LOG_ERROR("Unknown conversion mode.");
        success = false;
        break;
    }

    // 5. 결과 리턴
    int exitCode = 0;
    if (success)
    {
        MGF_LOG_INFO(">>> [SUCCESS] Processing finished.");
        exitCode = 0;
    }
    else
    {
        MGF_LOG_ERROR(">>> [FAILED] Processing failed or not implemented.");
        exitCode = 1;
    }

    // 6. 스레드 매니저 종료
    MGF_THREAD.Shutdown();

    return exitCode;
}