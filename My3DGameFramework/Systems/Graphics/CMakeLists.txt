# 0. 라이브러리 이름
set(MODULE_NAME Graphics) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # Animation
    "Animation/AnimChannel.h"       "Animation/AnimChannel.cpp"
    "Animation/AnimClip.h"          "Animation/AnimClip.cpp"
    "Animation/Pose.h"              "Animation/Pose.cpp"
    "Animation/Skeleton.h"          "Animation/Skeleton.cpp"

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
    Engine::Core
)

# 6. PCH (Precompiled Header) 설정
# 옵션 A: 이 라이브러리만의 개별 PCH를 사용할 경우
target_precompile_headers(${LIB_NAME} PRIVATE "RenderingPch.h")

# 옵션 B: CoreLib에 정의된 PCH를 재사용할 경우 (빌드 속도 최적화)
# target_precompile_headers(${LIB_NAME} REUSE_FROM CoreLib)