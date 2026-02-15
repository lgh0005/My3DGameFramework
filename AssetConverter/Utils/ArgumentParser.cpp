#include "pch.h"
#include "ArgumentParser.h"

ParseResult ArgumentParser::Parse(int argc, char* argv[])
{
	ParseResult result;
	if (argc < 2) return result;

	result.flipY = true;

	// 0. 첫 번째 인자를 통해서 현재 어떤 명령이 들어왔는 지를 체크
	std::string command = argv[1];

	// 1. verify 단계
	if (command == "--check")
	{
		result.mode = ConversionMode::Verify;
		return result;
	}

	// 2. [Model] -m <in> <out> [--extract-orm] [--no-flip]
	else if (command == "-m" || command == "--model")
	{
		if (argc < 4) return result;
		result.mode = ConversionMode::Model;
		result.inputPath = argv[2];
		result.outputPath = argv[3];
		result.extractOrm = false;
		for (int i = 4; i < argc; ++i)
		{
			std::string opt = argv[i];
			if (opt == "--extract-orm") result.extractOrm = true;
			if (opt == "--no-flip")      result.flipY = false;
		}
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

	// 4. [ORM] --orm <ao_or_none> <rough_or_none> <metal_or_none> <out_png> [--invert-roughness] [--no-flip]
	else if (command == "--orm")
	{
		if (argc < 6) return result;

		auto norm = [](const char* arg) -> std::string 
		{
			if (!arg) return {}; 
			std::string s = arg;
			if (s == "none" || s == "-") return {};
			return s;
		};

		result.mode = ConversionMode::ORM;
		result.aoMapPath = norm(argv[2]);
		result.roughnessMapPath = norm(argv[3]);
		result.metallicMapPath = norm(argv[4]);
		result.outputPath = argv[5];
		result.invertRoughness = false;
		for (int i = 6; i < argc; ++i)
		{
			std::string opt = argv[i];
			if (opt == "--invert-roughness") result.invertRoughness = true;
			if (opt == "--no-flip")      result.flipY = false;
		}

		// out 인자 유무 점검
		if (result.outputPath.empty()) return ParseResult{};

		// 최소 1개 입력은 있는지 점검
		if (result.aoMapPath.empty() && 
			result.roughnessMapPath.empty() && 
			result.metallicMapPath.empty()) return ParseResult{};
	}

	// 5. [KTX] -ktx <in_image> <out_ktx> [format] [--no-flip]
	else if (command == "--ktx")
	{
		if (argc < 6) return result;

		result.mode = ConversionMode::KTX;
		result.inputPath = argv[2];
		result.outputPath = argv[3];
		result.ktxFormat = argv[4];
		result.ktxColorSpace = argv[5];

		for (int i = 6; i < argc; ++i)
		{
			std::string opt = argv[i];
			if (opt == "--no-flip") result.flipY = false;
		}
	}

	// 6. [CubeMap] --cubemap <px> <f_px> <nx> <f_nx> <py> <f_py> <ny> <f_ny> <pz> <f_pz> <nz> <f_nz> <out>
	else if (command == "--cubemap")
	{
		if (argc < 22) return result;

		result.mode = ConversionMode::CubeMap;
		result.cubeFaces.reserve(6);

		// 6쌍의 (경로, 플립) 파싱
		for (int32 i = 0; i < 6; ++i)
		{
			CubeFaceElement face;
			int32 baseIdx = 2 + (i * 3);
			face.path = argv[baseIdx];
			face.flipX = (std::string(argv[baseIdx + 1]) == "1");
			face.flipY = (std::string(argv[baseIdx + 2]) == "1");
			result.cubeFaces.push_back(face);
		}

		result.outputPath = argv[20];
		result.isSRGB = (std::string(argv[21]) == "1");
	}

	return result;
}

void ArgumentParser::PrintUsage()
{
	std::cout << "\n[ Usage ]\n";
	std::cout << "  1. Verify:\n";
	std::cout << "     AssetConverter.exe --check\n\n";

	std::cout << "  2. Model Conversion:\n";
	std::cout << "     AssetConverter.exe -m <input.fbx> <output.mymodel>\n\n";

	std::cout << "  3. Animation Conversion:\n";
	std::cout << "     AssetConverter.exe -a <anim.fbx> <ref_model.mymodel> <out.myanim>\n\n";

	std::cout << "  4. ORM Texture Packing:\n";
	std::cout << "     AssetConverter.exe --orm <ao_or_none> <rough_or_none> <metal_or_none> <out.png>\n";
	std::cout << "       - Use 'none' (or '-') for missing maps.\n";
	std::cout << "       - At least one of AO/Rough/Metal must be provided.\n";
	std::cout << "       - Defaults when missing: AO=1.0 (white), Rough=1.0 (white), Metal=0.0 (black)\n";

	std::cout << "  5. KTX Texture Conversion:\n";
	std::cout << "     AssetConverter.exe --ktx <input.png> <output.ktx> <format>\n";
	std::cout << "       - Required formats: BC7, BC3, BC1, RGBA8\n";
	std::cout << "       - Color Spaces: sRGB (for Color/Albedo), Linear (for Normal/Data)\n";

	std::cout << "\n  * Options:\n";
	std::cout << "    --no-flip : Disable Y-axis flipping (Default: Enabled for OpenGL)\n";

	std::cout << "  6. CubeMap Conversion:\n";
	std::cout << "     AssetConverter.exe --cubemap <px> <f_px> <nx> <f_nx> ... <nz> <f_nz> <out.ktx> <isSRGB>\n";
	std::cout << "       - f_px~f_nz: 1 to enable FlipY, 0 to disable.\n";
	std::cout << "       - Order: PositiveX, NegativeX, PositiveY, NegativeY, PositiveZ, NegativeZ\n";

	std::cout << "\n";
}



