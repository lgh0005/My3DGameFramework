# 0. 라이브러리 이름
set(MODULE_NAME Systems) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # pch
    "SystemPch.h"

    # Managers

    # Audios
    
    # Graphics
    "Graphics/Buffers/GLBufferHandle.h"             "Graphics/Buffers/GLBufferHandle.cpp"
    "Graphics/Buffers/GLIndexBuffer.h"              "Graphics/Buffers/GLIndexBuffer.cpp"
    "Graphics/Buffers/GLUniformBuffer.h"            "Graphics/Buffers/GLUniformBuffer.cpp"
    "Graphics/Buffers/GLShaderStorageBuffer.h"      "Graphics/Buffers/GLShaderStorageBuffer.cpp"
    "Graphics/Buffers/GLPixelBuffer.h"              "Graphics/Buffers/GLPixelBuffer.cpp"
    "Graphics/Buffers/GLTextureBuffer.h"            "Graphics/Buffers/GLTextureBuffer.cpp"
    "Graphics/Layout/GLVertexLayout.h"              "Graphics/Layout/GLVertexLayout.cpp"
    "Graphics/Textures/GLTextureHandle.h"           "Graphics/Textures/GLTextureHandle.cpp"
    "Graphics/Textures/GLTexture2D.h"               "Graphics/Textures/GLTexture2D.cpp"
    "Graphics/Textures/GLTexture3D.h"               "Graphics/Textures/GLTexture3D.cpp"
    "Graphics/Textures/GLTextureCube.h"             "Graphics/Textures/GLTextureCube.cpp"
    "Graphics/Meshes/Mesh.h"                        "Graphics/Meshes/Mesh.cpp"
    "Graphics/Meshes/ScreenMesh.h"                  "Graphics/Meshes/ScreenMesh.cpp"
    "Graphics/Meshes/StaticMesh.h"                  "Graphics/Meshes/StaticMesh.cpp"
    "Graphics/Meshes/SkinnedMesh.h"                 "Graphics/Meshes/SkinnedMesh.cpp"
    #"Graphics/Meshes/DebugMesh.h"                   "Graphics/Meshes/DebugMesh.cpp"
    #"Graphics/Meshes/InstancedMesh.h"               "Graphics/Meshes/InstancedMesh.cpp"
    #"Graphics/Meshes/UIMesh.h"                      "Graphics/Meshes/UIMesh.cpp"
    "Graphics/Programs/Program.h"                    "Graphics/Programs/Program.cpp"
    "Graphics/Uniforms/Uniform.h"                    "Graphics/Uniforms/Uniform.cpp"
    "Graphics/Shader/Shader.h"                      "Graphics/Shader/Shader.cpp"
    "Graphics/Vertices/ScreenVertex.h" 
    "Graphics/Vertices/SkinnedVertex.h" 
    "Graphics/Vertices/StaticVertex.h" 

    # Physics

    # Scripting

    # Utils
    "SystemUtils/TextureUtils.h"        "SystemUtils/TextureUtils.cpp"
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
    Engine::Core
)

# 6. PCH (Precompiled Header) 설정
target_precompile_headers(${LIB_NAME} PRIVATE "SystemPch.h")