#pragma once

enum class ConversionMode
{
    None = 0,
    Verify,     // --check (Python 연동 확인용)
    Model,      // -m
    Animation,  // -a
    ORM         // -orm
};

struct ParseResult
{
    ConversionMode mode = ConversionMode::None;

    // [공통] 출력 경로 (파일 전체 경로)
    std::string outputPath;

    // [Model]
    std::string inputPath;
    bool extractOrm = false;

    // [Animation] 애니메이션의 기준이 되는 모델 파일
    std::string refModelPath;

    // [ORM Texture] 개별 PBR 텍스처 패킹용
    std::string aoMapPath;
    std::string roughnessMapPath;
    std::string metallicMapPath;
    bool invertRoughness = false;

    bool IsValid() const { return mode != ConversionMode::None; }
};

class ArgumentParser
{
public:
    static ParseResult Parse(int argc, char* argv[]);
    static void PrintUsage();
};