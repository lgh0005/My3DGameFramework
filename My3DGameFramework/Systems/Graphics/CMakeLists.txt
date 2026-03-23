# 0. 라이브러리 이름
set(MODULE_NAME Graphics) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # Module
    "GraphicsModule.h"              "GraphicsModule.cpp"

    # Common
    "Common/GraphicsConstants.h"
    "Common/AssetFormat.h"          "Common/AssetFormat.cpp"

    # Core
    # "Core/GlobalUniforms.h"         "Core/GlobalUniforms.cpp"
    # "Core/RenderContext.h"          "Core/RenderContext.cpp"
    # "Core/Renderer.h"               "Core/Renderer.cpp"
    # "Core/RenderPass.h"             "Core/RenderPass.cpp"
    # "Core/RenderPipeline.h"         "Core/RenderPipeline.cpp"

    # Debug
    "Debug/VRAMProfiler.h"          "Debug/VRAMProfiler.cpp"

    # Defines
    "Defines/DefineManager.h"
    "Defines/DefineMemoryAlignment.h"
    "Defines/DefineGLExtension.h"

    # Animation
    # "Animation/AnimChannel.h"       "Animation/AnimChannel.cpp"
    # "Animation/AnimClip.h"          "Animation/AnimClip.cpp"
    # "Animation/Pose.h"              "Animation/Pose.cpp"
    # "Animation/Skeleton.h"          "Animation/Skeleton.cpp"

    # Uniforms
    "Uniforms/CameraUBO.h"           "Uniforms/CameraUBO.cpp"
    "Uniforms/LightUBO.h"            "Uniforms/LightUBO.cpp"
    "Uniforms/LightArrayUBO.h"       "Uniforms/LightArrayUBO.cpp"
    "Uniforms/MaterialFactorUBO.h"   "Uniforms/MaterialFactorUBO.cpp"

    # Vertices
    "Vertices/ScreenVertex.h"        "Vertices/ScreenVertex.cpp"
    "Vertices/SkinnedVertex.h"       "Vertices/SkinnedVertex.cpp"
    "Vertices/StaticVertex.h"        "Vertices/StaticVertex.cpp"
    "Vertices/UIVertex.h"            "Vertices/UIVertex.cpp"

    # Meshes
    "Meshes/Mesh.h"                  "Meshes/Mesh.cpp"
    "Meshes/SubMesh.h"               "Meshes/SubMesh.cpp"

    # Shaders

    # Hashing
    "Hashing/TextureHash.h"             "Hashing/TextureHash.cpp"           "Hashing/TextureHash.inl"
    "Hashing/FramebufferHash.h"         "Hashing/FramebufferHash.cpp"       "Hashing/FramebufferHash.inl"

    # Buffers
    "Buffers/GLBufferHandle.h"          "Buffers/GLBufferHandle.cpp"
    "Buffers/GLIndexBuffer.h"           "Buffers/GLIndexBuffer.cpp"
    "Buffers/GLShaderStorageBuffer.h"   "Buffers/GLShaderStorageBuffer.cpp"
    "Buffers/GLUniformBuffer.h"         "Buffers/GLUniformBuffer.cpp"
    "Buffers/GLVertexBuffer.h"          "Buffers/GLVertexBuffer.cpp"

    # Textures
    "Textures/GLTexture.h"                 "Textures/GLTexture.cpp"
    "Textures/GLTextureCube.h"             "Textures/GLTextureCube.cpp"
    "Textures/GLTexture2D.h"               "Textures/GLTexture2D.cpp"
    "Textures/GLTexture3D.h"               "Textures/GLTexture3D.cpp"

    # Framebuffers
    "Framebuffers/GLFramebuffer.h"        "Framebuffers/GLFramebuffer.cpp"
    "Framebuffers/GLFramebufferCube.h"    "Framebuffers/GLFramebufferCube.cpp"
    "Framebuffers/GLFramebuffer2D.h"      "Framebuffers/GLFramebuffer2D.cpp"
    "Framebuffers/GLFramebuffer3D.h"      "Framebuffers/GLFramebuffer3D.cpp"

    # Instancing
    # "Instancing/AnimBuffer.h"           "Instancing/AnimBuffer.cpp"
    # "Instancing/InstanceBuffer.h"       "Instancing/InstanceBuffer.cpp"
    # "Instancing/InstanceProperty.h"     "Instancing/InstanceProperty.cpp"
    # "Instancing/RenderBatch.h"          "Instancing/RenderBatch.cpp"
    # "Instancing/RenderQueue.h"          "Instancing/RenderQueue.cpp"

    # Layouts
    "Layouts/VertexLayout.h"             "Layouts/VertexLayout.cpp"

    # VRAM
    "VRAM/VRAMAllocation.h"           "VRAM/VRAMAllocation.cpp"       
    "VRAM/VRAMFreeList.h"           "VRAM/VRAMFreeList.cpp"
    "VRAM/VRAMNode.h"               "VRAM/VRAMNode.cpp"
    "VRAM/VRAMPool.h"               "VRAM/VRAMPool.cpp"

    # Pools
    "Pools/FramebufferPool.h"           "Pools/FramebufferPool.cpp"         "Pools/FramebufferPool.inl"
    "Pools/Framebuffer2DPool.h"           "Pools/Framebuffer2DPool.cpp" 
    "Pools/Framebuffer3DPool.h"           "Pools/Framebuffer3DPool.cpp" 
    "Pools/FramebufferCubePool.h"           "Pools/FramebufferCubePool.cpp"

    "Pools/TexturePool.h"           "Pools/TexturePool.cpp"         "Pools/TexturePool.inl"
    "Pools/Texture2DPool.h"           "Pools/Texture2DPool.cpp" 
    "Pools/Texture3DPool.h"           "Pools/Texture3DPool.cpp" 
    "Pools/TextureCubePool.h"           "Pools/TextureCubePool.cpp"

    # Managers
    "Managers/VRAMManager.h"          "Managers/VRAMManager.cpp"
    "Managers/TextureManager.h"       "Managers/TextureManager.cpp"
    "Managers/FramebufferManager.h"   "Managers/FramebufferManager.cpp"
    "Managers/RenderManager.h"        "Managers/RenderManager.cpp"    # "Managers/RenderManager.inl"
    # "Managers/InstanceManager.h"      "Managers/InstanceManager.cpp"

    # Utils
    # "Utils/GeometryGenerator.h"        "Utils/GeometryGenerator.cpp"
    # "Utils/IBLUtils.h"                 "Utils/IBLUtils.cpp"
    "Utils/SubMeshBuilder.h"             "Utils/SubMeshBuilder.cpp"     "Utils/SubMeshBuilder.inl"
    "Utils/TextureUtils.h"               "Utils/TextureUtils.cpp"
)

# 2. 빌드 타겟 생성 및 별칭 등록
add_library(${LIB_NAME} STATIC ${SOURCES})
add_library(${TARGET_NAME} ALIAS ${LIB_NAME})

# 3. Visual Studio 필터 설정
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# 4. Include 경로 설정
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 5. 의존성 링크
target_link_libraries(${LIB_NAME} PUBLIC

PRIVATE
    # KTX::ktx
    # assimp::assimp
    glad::glad
    glfw
    Engine::Core
)

# 6. PCH (Precompiled Header) 설정
# 옵션 A: 이 라이브러리만의 개별 PCH를 사용할 경우
target_precompile_headers(${LIB_NAME} PRIVATE "GraphicsPch.h")

# 옵션 B: CoreLib에 정의된 PCH를 재사용할 경우 (빌드 속도 최적화)
# target_precompile_headers(${LIB_NAME} REUSE_FROM CoreLib)