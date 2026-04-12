#pragma once

namespace MGF3D
{
    MGF_CLASS_PTR(Application)
    class Application
    {
        MGF_DECLARE_UTILITIES(Application)

    public:
        static bool Init
        (
            int32 width, int32 height, 
            const String& title, int32 vsync = 0
        );
        static void Run();
        static bool Shutdown();
    };
}