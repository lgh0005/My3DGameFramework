find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(KTX CONFIG REQUIRED)

add_library(Engine STATIC

    # PCH
   "EnginePch.h"                    "EnginePch.cpp"

   # Core
   "Core/Application.h"             "Core/Application.cpp"
   "Core/GameObject.h"              "Core/GameObject.cpp"
   "Core/Scene.h"                   "Core/Scene.cpp"
   "Core/Renderer.h"                "Core/Renderer.cpp"
   "Core/RenderPass.h"              "Core/RenderPass.cpp"
   "Core/RenderPipeline.h"          "Core/RenderPipeline.cpp"
   "Core/RenderContext.h"           "Core/RenderContext.cpp"

   # Common Render Passes
   "Pipelines/Common/CullingPass.h" "Pipelines/Common/CullingPass.cpp"
   "Pipelines/Common/SkyboxPass.h"  "Pipelines/Common/SkyboxPass.cpp"
   "Pipelines/Common/SSAOPass.h"    "Pipelines/Common/SSAOPass.cpp"
   "Pipelines/Common/ShadowPass.h"  "Pipelines/Common/ShadowPass.cpp"

   # Standard Render Pipeline
   "Pipelines/SRP/RenderPasses/StandardPostProcessPass.h" "Pipelines/SRP/RenderPasses/StandardPostProcessPass.cpp"
   "Pipelines/SRP/RenderPasses/StandardGeometryPass.h"  "Pipelines/SRP/RenderPasses/StandardGeometryPass.cpp"
   "Pipelines/SRP/RenderPasses/StandardDeferredLightingPass.h"  "Pipelines/SRP/RenderPasses/StandardDeferredLightingPass.cpp"
   "Pipelines/SRP/StandardGlobalUniforms.h"    "Pipelines/SRP/StandardGlobalUniforms.cpp"
   "Pipelines/SRP/StandardRenderContext.h"     "Pipelines/SRP/StandardRenderContext.cpp"
   "Pipelines/SRP/StandardRenderPipeline.h"    "Pipelines/SRP/StandardRenderPipeline.cpp"

   # Universal Render Pipeline
   "Pipelines/URP/UniversalRenderPipeline.h"   "Pipelines/URP/UniversalRenderPipeline.cpp"
   "Pipelines/URP/UniversalGlobalUniforms.h"   "Pipelines/URP/UniversalGlobalUniforms.cpp"
   "Pipelines/URP/UniversalRenderContext.h"    "Pipelines/URP/UniversalRenderContext.cpp"

   # Misc
   "Misc/Defines.h"
   "Misc/Types.h"
   "Misc/ImguiManager.h"            "Misc/ImguiManager.cpp"
   "Misc/Utils.h"                   "Misc/Utils.cpp"

   # Graphics
   "Graphics/Shader.h"              "Graphics/Shader.cpp"
   "Graphics/Program.h"             "Graphics/Program.cpp"
   "Graphics/Buffer.h"              "Graphics/Buffer.cpp" 
   "Graphics/VertexLayout.h"        "Graphics/VertexLayout.cpp"
   "Graphics/Image.h"               "Graphics/Image.cpp"
   "Graphics/Texture.h"             "Graphics/Texture.cpp" 
   "Graphics/Mesh.h"                "Graphics/Mesh.cpp"
   "Graphics/StaticMesh.h"          "Graphics/StaticMesh.cpp" 
   "Graphics/SkinnedMesh.h"         "Graphics/SkinnedMesh.cpp"
   "Graphics/InstancedMesh.h"       "Graphics/InstancedMesh.cpp"
   "Graphics/Vertex.h"              "Graphics/Vertex.cpp"
   "Graphics/Model.h"               "Graphics/Model.cpp"
   "Graphics/Material.h"            "Graphics/Material.cpp"
   "Graphics/Bone.h"                "Graphics/Bone.cpp"
   "Graphics/Animation.h"           "Graphics/Animation.cpp"
   "Graphics/FrameBuffer.h"         "Graphics/FrameBuffer.cpp"
   "Graphics/CubeTexture.h"         "Graphics/CubeTexture.cpp"
   "Graphics/ShadowMap.h"           "Graphics/ShadowMap.cpp"
   "Graphics/UniformBuffer.h"       "Graphics/UniformBuffer.cpp"
   "Graphics/Uniform.h"             "Graphics/Uniform.cpp"
   "Graphics/Frustum.h"             "Graphics/Frustum.cpp"
   "Graphics/Geometry.h"            "Graphics/Geometry.cpp"
   "Graphics/CubeFramebuffer.h"     "Graphics/CubeFramebuffer.cpp"

   # Audio
   "Audios/AudioClip.h"             "Audios/AudioClip.cpp"

   # Components
   "Components/Component.h"         "Components/Component.cpp"
   "Components/Light.h"             "Components/Light.cpp"
   "Components/Script.h"            "Components/Script.cpp"

   "Components/Camera.h"            "Components/Camera.cpp"
   "Components/Transform.h"         "Components/Transform.cpp"
   "Components/DirectionalLight.h"  "Components/DirectionalLight.cpp"
   "Components/PointLight.h"        "Components/PointLight.cpp"
   "Components/SpotLight.h"         "Components/SpotLight.cpp"
   "Components/Animator.h"          "Components/Animator.cpp"
   "Components/MeshRenderer.h"      "Components/MeshRenderer.cpp"
   "Components/AudioSource.h"       "Components/AudioSource.cpp"
   "Components/AudioListener.h"       "Components/AudioListener.cpp"

   # Managers
   "Managers/TimeManager.h"         "Managers/TimeManager.cpp"
   "Managers/WindowManager.h"       "Managers/WindowManager.cpp"
   "Managers/InputManager.h"        "Managers/InputManager.cpp"           
   "Managers/ResourceManager.h"     "Managers/ResourceManager.cpp"
   "Managers/SceneManager.h"        "Managers/SceneManager.cpp"
   "Managers/RenderManager.h"       "Managers/RenderManager.cpp"
   "Managers/AudioManager.h"        "Managers/AudioManager.cpp"
 
)

# 2. [PCH 설정] EnginePCH.h를 PCH로 사용합니다.
target_precompile_headers(Engine  PUBLIC EnginePCH.h)

# 3. [헤더 경로] 다른 프로젝트(Game)가 Engine의 헤더 파일을
#    찾을 수 있도록 현재 폴더를 PUBLIC include 경로로 지정합니다.
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 4. 외부 라이브러리를 여기에 링크
target_link_libraries(Engine PUBLIC
    glad::glad
    glfw
    glm::glm
    imgui::imgui
    spdlog::spdlog
    assimp::assimp
    KTX::ktx
)

# 5. [컴파일 정의] Engine 타겟에 C++ 매크로를 정의합니다.
target_compile_definitions(Engine PUBLIC
    WINDOW_WIDTH=1200
    WINDOW_HEIGHT=800
    WINDOW_NAME="My3DGameFramework"
)