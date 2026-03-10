#pragma once

namespace MGF3D
{
    struct GLFWMemHeader { usize size; };

    class GLFWAllocateHelper
    {
        MGF_DECLARE_UTILITIES(GLFWAllocateHelper)

    public:
        static void* GLFW_Allocate(usize size, void*);
        static void GLFW_Deallocate(void* block, void* user);
        static void* GLFW_Reallocate(void* block, usize size, void* user);
    };
}