#include "pch.h"
#include "Converters/ModelConverter.h"
#include "Converters/AnimConverter.h"

void PrintUsage()
{
    std::cout << "\n[ Usage ]\n";
    std::cout << "  AssetConverter.exe <option> <arguments...>\n\n";
    std::cout << "[ Options ]\n";
    std::cout << "  -m, --model <input model> <output file>\n";
    std::cout << "      Converts a model file (.fbx, .obj) to .mymodel format.\n";
    std::cout << "      Example: AssetConverter.exe -m backpack.obj backpack.mymodel\n\n";
    std::cout << "  -a, --anim <input animation> <base model> <output file>\n";
    std::cout << "      Converts an animation file (.fbx) to .myanim format.\n";
    std::cout << "      (Requires a base .mymodel file as reference!)\n";
    std::cout << "      Example: AssetConverter.exe -a run.fbx character.mymodel run.myanim\n";
}

int main(int argc, char* argv[])
{
    spdlog::set_level(spdlog::level::info);

    if (argc < 2) // Check if at least an option is provided
    {
        SPDLOG_ERROR("Not enough arguments.");
        PrintUsage();
        return -1;
    }

    std::string option = argv[1];
    bool success = false;

    try
    {
        if (option == "-m" || option == "--model")
        {
            // Model conversion: needs 4 arguments (exe, option, input, output)
            if (argc < 4)
            {
                SPDLOG_ERROR("Model conversion requires input and output file paths.");
                PrintUsage();
                return -1;
            }
            std::string inputPath = argv[2];
            std::string outputPath = argv[3];

            SPDLOG_INFO("=== Model Conversion Mode ===");
            success = ModelConverter::Convert(inputPath, outputPath);
        }
        else if (option == "-a" || option == "--anim")
        {
            // Animation conversion: needs 5 arguments (exe, option, animInput, modelInput, output)
            if (argc < 5)
            {
                SPDLOG_ERROR("Animation conversion requires animation file, base model file, and output file paths.");
                PrintUsage();
                return -1;
            }
            std::string animInputPath = argv[2];
            std::string modelInputPath = argv[3];
            std::string outputPath = argv[4];

            // Check if input files exist
            if (!std::filesystem::exists(animInputPath))
            {
                SPDLOG_ERROR("Cannot find animation file: {}", animInputPath);
                return -1;
            }
            if (!std::filesystem::exists(modelInputPath))
            {
                SPDLOG_ERROR("Cannot find base model file: {}", modelInputPath);
                return -1;
            }

            SPDLOG_INFO("=== Animation Conversion Mode ===");
            success = AnimConverter::Convert(animInputPath, modelInputPath, outputPath);
        }
        else
        {
            SPDLOG_ERROR("Unknown option: {}", option);
            PrintUsage();
            return -1;
        }
    }
    catch (const std::exception& e)
    {
        SPDLOG_ERROR("Exception occurred: {}", e.what());
        return -1;
    }

    if (success)
    {
        SPDLOG_INFO(">>> Conversion Complete! <<<");
        return 0;
    }
    else
    {
        SPDLOG_ERROR(">>> Conversion Failed! <<<");
        return -1;
    }
}