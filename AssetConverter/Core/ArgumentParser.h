#pragma once
#include "Core/ParseResult.h"

namespace MGF3D
{
    class ArgumentParser
    {
        MGF_DECLARE_UTILITIES(ArgumentParser)

    public:
        static ParseResult Parse(int argc, char* argv[]);
        static void PrintUsage();
    };
}
