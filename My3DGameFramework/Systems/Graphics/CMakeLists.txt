# 0. 라이브러리 이름
set(MODULE_NAME Graphics) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # pch
    "GraphicsPch.h"

    # Module
    "GraphicsModule.h"                     "GraphicsModule.cpp"

    # Constants
    "Constants/ConstantGraphics.h"

    # Managers
    
    # Assets
    "Assets/Shader.h"                      "Assets/Shader.cpp"
    "Assets/Image.h"                       "Assets/Image.cpp"

    # Buffer
    "Buffers/GLBufferHandle.h"             "Buffers/GLBufferHandle.cpp"
    "Buffers/GLVertexBuffer.h"             "Buffers/GLVertexBuffer.cpp"
    "Buffers/GLIndexBuffer.h"              "Buffers/GLIndexBuffer.cpp"
    "Buffers/GLUniformBuffer.h"            "Buffers/GLUniformBuffer.cpp"
    "Buffers/GLShaderStorageBuffer.h"      "Buffers/GLShaderStorageBuffer.cpp"
    "Buffers/GLPixelBuffer.h"              "Buffers/GLPixelBuffer.cpp"
    "Buffers/GLTextureBuffer.h"            "Buffers/GLTextureBuffer.cpp"

    # Framebuffers
    "Framebuffers/GLFramebufferHandle.h"    "Framebuffers/GLFramebufferHandle.cpp"
    "Framebuffers/GLFramebuffer2D.h"    "Framebuffers/GLFramebuffer2D.cpp"
    "Framebuffers/GLFramebuffer3D.h"    "Framebuffers/GLFramebuffer3D.cpp"
    "Framebuffers/GLFramebufferCube.h"    "Framebuffers/GLFramebufferCube.cpp"

    # Layout
    "Layout/GLVertexLayout.h"              "Layout/GLVertexLayout.cpp"

    # Textures
    "Textures/GLTextureHandle.h"           "Textures/GLTextureHandle.cpp"
    "Textures/GLTexture2D.h"               "Textures/GLTexture2D.cpp"
    "Textures/GLTexture3D.h"               "Textures/GLTexture3D.cpp"
    "Textures/GLTextureCube.h"             "Textures/GLTextureCube.cpp"

    # Geometry
    "Geometry/CullingPlane.h"              "Geometry/CullingPlane.cpp"
    "Geometry/Frustum.h"                   "Geometry/Frustum.cpp"   
    "Geometry/RenderBounds.h"              "Geometry/RenderBounds.cpp"      "Geometry/RenderBounds.inl"

    # Meshes
    "Meshes/Mesh.h"                        "Meshes/Mesh.cpp"
    "Meshes/ScreenMesh.h"                  "Meshes/ScreenMesh.cpp"
    "Meshes/StaticMesh.h"                  "Meshes/StaticMesh.cpp"
    "Meshes/SkinnedMesh.h"                 "Meshes/SkinnedMesh.cpp"

    # Program
    "Programs/Program.h"                    "Programs/Program.cpp"
    "Programs/GraphicsProgram.h"            "Programs/GraphicsProgram.cpp"
    "Programs/ComputeProgram.h"             "Programs/ComputeProgram.cpp"

    # Shader
    "Shader/GLShader.h"                      "Shader/GLShader.cpp"

    # Vertices
    "Vertices/ScreenVertex.h" 
    "Vertices/SkinnedVertex.h" 
    "Vertices/StaticVertex.h" 

    # Uniforms
    "Uniforms/LightUniform.h"
    "Uniforms/CameraUniform.h"
    "Uniforms/MaterialUniform.h"

    # Utils
    "GraphicsUtils/GeometryUtils.h"       "GraphicsUtils/GeometryUtils.cpp"
    # "GraphicsUtils/IBLUtils.h"            "GraphicsUtils/IBLUtils.cpp"
    "GraphicsUtils/TextureUtils.h"        "GraphicsUtils/TextureUtils.cpp"
)

# 2. 빌드 타겟 생성 및 별칭 등록
add_library(${LIB_NAME} STATIC ${SOURCES})
add_library(${TARGET_NAME} ALIAS ${LIB_NAME})

# 3. Visual Studio 필터 설정
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# 4. Include 경로 설정
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 5. 의존성 링크 : miniaudio.h
target_link_libraries(${LIB_NAME} 

PUBLIC
    KTX::ktx
    Engine::Core
)

# 6. PCH (Precompiled Header) 설정
target_precompile_headers(${LIB_NAME} PRIVATE "GraphicsPch.h")