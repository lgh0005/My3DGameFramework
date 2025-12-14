#include "pch.h"
#include "Utils/ArgumentParser.h"

int main(int argc, char* argv[])
{
    // Logger 초기화
    LOGGER.Init(argc);

    // 2. 인자 파싱 (ArgumentParser 이용)
    ParseResult args = ArgumentParser::Parse(argc, argv);

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
    case ConversionMode::Verify:

        // Python GUI가 "--check"를 보냈을 때 "AssetConverter verified" 문자열을 기다림
        std::cout << "AssetConverter verified" << std::endl;
        return 0;
    
    // [Model Mode]
    case ConversionMode::Model:
        LOG_INFO(">>> [Mode] Model Conversion Selected");
        success = CONV_MODEL.Convert(args.inputPath, args.outputPath);
        break;

    // [Animation Mode]
    case ConversionMode::Animation:
        LOG_INFO(">>> [Mode] Animation Conversion Selected");

        // TODO: AnimConverter 구현 후 연결
        // success = AnimConverter::Instance().Convert(args.inputPath, args.refModelPath, args.outputPath);

        LOG_WARN("Animation converter is under construction.");
        success = false; // 아직 구현 안 됐으므로 실패 처리
        break;

    // [ORM Texture Mode]
    case ConversionMode::ORM:
        LOG_INFO(">>> [Mode] ORM Texture Packing Selected");
        success = CONV_ORM.Convert(args.aoMapPath, args.roughnessMapPath, args.metallicMapPath, args.outputPath);
        break;

    default:
        LOG_ERROR("Unknown conversion mode.");
        success = false;
        break;
    }

    // 5. 결과 리턴
    if (success)
    {
        LOG_INFO(">>> [SUCCESS] Processing finished.");
        return 0;
    }
    else
    {
        LOG_ERROR(">>> [FAILED] Processing failed or not implemented.");
        return 1;
    }
}