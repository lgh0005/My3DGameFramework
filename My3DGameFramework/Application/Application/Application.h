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

    private:
        // [DEBUG] 테스트를 위한 임시 렌더링 자원들
        // 실제 상용 환경에서는 Scene이나 Renderer 내부에서 관리되어야 합니다.
        static StaticMeshPtr      g_debugCube;
        static GraphicsProgramPtr g_debugProgram;

        // 비동기 에셋 참조 유지를 위한 포인터 (수명 관리용)
        static ShaderPtr  g_debugVS;
        static ShaderPtr  g_debugFS;
    };
}