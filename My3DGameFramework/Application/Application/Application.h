#pragma once

namespace MGF3D
{
    MGF_CLASS_PTR(StaticMesh)
    MGF_CLASS_PTR(GraphicsProgram)
    MGF_CLASS_PTR(Shader)

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