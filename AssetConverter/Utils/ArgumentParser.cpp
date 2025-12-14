#include "pch.h"
#include "ArgumentParser.h"

ParseResult ArgumentParser::Parse(int argc, char* argv[])
{
	ParseResult result;
	if (argc < 2) return result;

	// 0. 첫 번째 인자를 통해서 현재 어떤 명령이 들어왔는 지를 체크
	std::string command = argv[1];

	// 1. verify 단계
	if (command == "--check")
	{
		result.mode = ConversionMode::Verify;
		return result;
	}

	// 2. [Model] -m <in> <out>
	else if (command == "-m" || command == "--model")
	{
		if (argc < 4) return result;
		result.mode = ConversionMode::Model;
		result.inputPath = argv[2];
		result.outputPath = argv[3];
	}

	// 3. [Animation] -a <anim_in> <ref_model> <out>
	else if (command == "-a" || command == "--anim")
	{
		if (argc < 5)
		{
			LOG_ERROR("Animation conversion requires: AnimationFile, BaseModel, OutputPath");
			return result;
		}
		result.mode = ConversionMode::Animation;
		result.inputPath = argv[2];    // 애니메이션 파일
		result.refModelPath = argv[3]; // 기준 모델 파일
		result.outputPath = argv[4];   // 출력 파일
	}

	// 4. [ORM] --t <ao> <rough> <metal> <out>
	else if (command == "--orm")
	{
		if (argc < 6) return result;
		result.mode = ConversionMode::ORM;
		result.aoMapPath = argv[2];
		result.roughnessMapPath = argv[3];
		result.metallicMapPath = argv[4];
		result.outputPath = argv[5];
	}

	return result;
}

void ArgumentParser::PrintUsage()
{
	std::cout << "\n[ Usage ]\n";
	std::cout << "  1. Verify:\n";
	std::cout << "     AssetConverter.exe --check\n";

	std::cout << "  2. Model Conversion:\n";
	std::cout << "     AssetConverter.exe -m <input.fbx> <output.mymodel>\n";

	std::cout << "  3. Animation Conversion:\n";
	std::cout << "     AssetConverter.exe -a <anim.fbx> <ref_model.mymodel> <out.myanim>\n";

	std::cout << "  4. ORM Texture Packing:\n";
	std::cout << "     AssetConverter.exe --orm <ao> <rough> <metal> <out.png>\n";
	std::cout << "\n";
}


