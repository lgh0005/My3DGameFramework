#pragma once

namespace MGF3D
{
    MGF_CLASS_PTR(Application)
    class Application
    {
        MGF_DECLARE_UTILITIES(Application)

    public:
        static void RegisterTypes();
        static bool Init();
        static void Run();
        static bool Shutdown();
    };
}