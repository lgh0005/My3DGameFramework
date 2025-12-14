#include "pch.h"
#include "Utils/ArgumentParser.h"
#include "Converters/ModelConverter.h"

// [TODO] 추후 구현 시 주석 해제
// #include "Converters/AnimConverter.h"
// #include "Converters/TextureConverter.h"

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
    case ConversionMode::Verify:
        // [Verify Mode]
        // Python GUI가 "--check"를 보냈을 때 "AssetConverter verified" 문자열을 기다림
        // 로그(LOG_INFO)는 포맷팅이 붙을 수 있으므로, 순수한 std::cout 사용 권장
        std::cout << "AssetConverter verified" << std::endl;
        return 0; // 즉시 성공 종료

    case ConversionMode::Model:
        // [Model Mode]
        // ModelConverter 싱글톤 호출
        LOG_INFO(">>> [Mode] Model Conversion Selected");
        success = CONV_MODEL.Convert(args.inputPath, args.outputPath);
        break;

    case ConversionMode::Animation:
        // [Animation Mode]
        LOG_INFO(">>> [Mode] Animation Conversion Selected");

        // TODO: AnimConverter 구현 후 연결
        // success = AnimConverter::Instance().Convert(args.inputPath, args.refModelPath, args.outputPath);

        LOG_WARN("Animation converter is under construction.");
        success = false; // 아직 구현 안 됐으므로 실패 처리
        break;

    case ConversionMode::ORM:
        // [ORM Texture Mode]
        LOG_INFO(">>> [Mode] ORM Texture Packing Selected");

        // TODO: TextureConverter 구현 후 연결
        // success = TextureConverter::Instance().ConvertORM(args.aoMapPath, args.roughnessMapPath, args.metallicMapPath, args.outputPath);

        LOG_WARN("ORM converter is under construction.");
        success = false; // 아직 구현 안 됐으므로 실패 처리
        break;

    default:
        LOG_ERROR("Unknown conversion mode.");
        success = false;
        break;
    }

    // 5. 결과 리턴
    // 0 : 성공 (subprocess.returncode == 0)
    // 1 : 실패 (subprocess.returncode != 0)
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