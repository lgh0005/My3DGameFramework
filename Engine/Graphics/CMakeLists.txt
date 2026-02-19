# 0. 라이브러리 이름
set(MODULE_NAME Graphics) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # Animations
    "Animations/Animation.h"     "Animations/Animation.cpp"
    "Animations/AnimChannel.h"     "Animations/AnimChannel.cpp"
    "Animations/AnimClip.h"     "Animations/AnimClip.cpp"
    "Animations/AnimController.h"     "Animations/AnimController.cpp"
    "Animations/AnimState.h"     "Animations/AnimState.cpp"
    "Animations/Pose.h"     "Animations/Pose.cpp"
    "Animations/Skeleton.h"     "Animations/Skeleton.cpp"

    # Buffers
    "Buffers/Buffer.h"      "Buffers/Buffer.cpp"
    "Buffers/IndexBuffer.h"      "Buffers/IndexBuffer.cpp"
    "Buffers/ShaderStorageBuffer.h"      "Buffers/ShaderStorageBuffer.cpp"
    "Buffers/UniformBuffer.h"      "Buffers/UniformBuffer.cpp"
    "Buffers/VertexBuffer.h"      "Buffers/VertexBuffer.cpp"

    # Framebuffers
    "Framebuffers/FramebufferBase.h"      "Framebuffers/FramebufferBase.cpp"

    "Framebuffers/Framebuffer.h"      "Framebuffers/Framebuffer.cpp"
    "Framebuffers/BRDFLookUpFramebuffer.h"      "Framebuffers/BRDFLookUpFramebuffer.cpp"
    "Framebuffers/CubeFramebuffer.h"      "Framebuffers/CubeFramebuffer.cpp"
    "Framebuffers/EffectFramebuffer.h"      "Framebuffers/EffectFramebuffer.cpp"
    "Framebuffers/GBufferFramebuffer.h"      "Framebuffers/GBufferFramebuffer.cpp"
    "Framebuffers/OutlineFramebuffer.h"      "Framebuffers/OutlineFramebuffer.cpp"
    "Framebuffers/PostProcessFrameBuffer.h"      "Framebuffers/PostProcessFrameBuffer.cpp"
    "Framebuffers/SSAOFramebuffer.h"      "Framebuffers/SSAOFramebuffer.cpp"

    # Geometry
    "Geometry/CullingPlane.h"      "Geometry/CullingPlane.cpp"
    "Geometry/Frustum.h"      "Geometry/Frustum.cpp"
    "Geometry/GeometryGenerator.h"      "Geometry/GeometryGenerator.cpp"
    "Geometry/RenderBounds.h"      "Geometry/RenderBounds.cpp"

    # Instancing
    "Instancing/AnimBuffer.h"      "Instancing/AnimBuffer.cpp"
    "Instancing/InstanceBuffer.h"      "Instancing/InstanceBuffer.cpp"
    "Instancing/InstanceProperty.h"      "Instancing/InstanceProperty.cpp"
    "Instancing/RenderBatch.h"      "Instancing/RenderBatch.cpp"
    "Instancing/RenderQueue.h"      "Instancing/RenderQueue.cpp"

    # Layouts
    "Layouts/Uniform.h"      "Layouts/Uniform.cpp"
    "Layouts/Vertex.h"      "Layouts/Vertex.cpp"
    "Layouts/VertexLayout.h"      "Layouts/VertexLayout.cpp"

    # Managers
    "Managers/InstanceManager.h"      "Managers/InstanceManager.cpp"
    "Managers/RenderManager.h"      "Managers/RenderManager.cpp"  "Managers/RenderManager.inl"

    # Meshes
    "Meshes/DebugMesh.h"      "Meshes/DebugMesh.cpp"
    "Meshes/InstancedMesh.h"      "Meshes/InstancedMesh.cpp"
    "Meshes/Mesh.h"      "Meshes/Mesh.cpp"
    "Meshes/ScreenMesh.h"      "Meshes/ScreenMesh.cpp"
    "Meshes/SkinnedMesh.h"      "Meshes/SkinnedMesh.cpp"
    "Meshes/StaticMesh.h"      "Meshes/StaticMesh.cpp"
    "Meshes/UIMesh.h"      "Meshes/UIMesh.cpp"

    # Programs
    "Programs/Program.h"      "Programs/Program.cpp"
    "Programs/GraphicsProgram.h"      "Programs/GraphicsProgram.cpp"
    "Programs/ComputeProgram.h"      "Programs/ComputeProgram.cpp"

    # Raycasting
    "Raycasting/Ray.h"              "Raycasting/Ray.cpp"
    "Raycasting/RaycastHit.h"      "Raycasting/RaycastHit.cpp"

    # Rendering
    "Rendering/GlobalUniforms.h"              "Rendering/GlobalUniforms.cpp"
    "Rendering/RenderContext.h"      "Rendering/RenderContext.cpp"
    "Rendering/Renderer.h"      "Rendering/Renderer.cpp"
    "Rendering/RenderPass.h"      "Rendering/RenderPass.cpp"
    "Rendering/RenderPipeline.h"      "Rendering/RenderPipeline.cpp"

    # Resources
    "Resources/EnvironmentMap.h"      "Resources/EnvironmentMap.cpp"
    "Resources/Material.h"      "Resources/Material.cpp"
    "Resources/Model.h"      "Resources/Model.cpp"
    "Resources/Shader.h"      "Resources/Shader.cpp"
    "Resources/ShadowMap.h"      "Resources/ShadowMap.cpp"
    "Resources/Velocitybuffer.h"      "Resources/Velocitybuffer.cpp"

    # Textures
    "Textures/TextureBase.h"      "Textures/TextureBase.cpp"

    "Textures/Texture.h"      "Textures/Texture.cpp"
    "Textures/CubeTexture.h"      "Textures/CubeTexture.cpp"

    # Utilities
    "Utilities/IBLUtils.h"      "Utilities/IBLUtils.cpp"
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
    KTX::ktx
    assimp::assimp
)

# 6. PCH (Precompiled Header) 설정
# 옵션 A: 이 라이브러리만의 개별 PCH를 사용할 경우
# target_precompile_headers(${LIB_NAME} PRIVATE "AiPch.h")

# 옵션 B: CoreLib에 정의된 PCH를 재사용할 경우 (빌드 속도 최적화)
# target_precompile_headers(${LIB_NAME} REUSE_FROM CoreLib)