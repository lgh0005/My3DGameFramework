find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(KTX CONFIG REQUIRED)
find_package(freetype REQUIRED)
find_package(Jolt CONFIG REQUIRED)

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
   "Pipelines/Common/ShadowPass.h"  "Pipelines/Common/ShadowPass.cpp"
   "Pipelines/Common/SkyboxPass.h"  "Pipelines/Common/SkyboxPass.cpp"
   "Pipelines/Common/OutlinePass.h" "Pipelines/Common/OutlinePass.cpp"

   # Standard Render Pipeline
   "Pipelines/SRP/RenderPasses/StandardPostProcessPass.h" "Pipelines/SRP/RenderPasses/StandardPostProcessPass.cpp"
   "Pipelines/SRP/RenderPasses/StandardGeometryPass.h"  "Pipelines/SRP/RenderPasses/StandardGeometryPass.cpp"
   "Pipelines/SRP/RenderPasses/StandardDeferredLightingPass.h"  "Pipelines/SRP/RenderPasses/StandardDeferredLightingPass.cpp"
   "Pipelines/SRP/RenderPasses/StandardSSAOPass.h"    "Pipelines/SRP/RenderPasses/StandardSSAOPass.cpp"
   "Pipelines/SRP/RenderPasses/StandardPostProcessPass.h"  "Pipelines/SRP/RenderPasses/StandardPostProcessPass.cpp"
   "Pipelines/SRP/StandardGlobalUniforms.h"    "Pipelines/SRP/StandardGlobalUniforms.cpp"
   "Pipelines/SRP/StandardRenderContext.h"     "Pipelines/SRP/StandardRenderContext.cpp"
   "Pipelines/SRP/StandardRenderPipeline.h"    "Pipelines/SRP/StandardRenderPipeline.cpp"

   # Universal Render Pipeline
   "Pipelines/URP/RenderPasses/UniversalGeometryPass.h" "Pipelines/URP/RenderPasses/UniversalGeometryPass.cpp"
   "Pipelines/URP/RenderPasses/UniversalPostProcessPass.h"  "Pipelines/URP/RenderPasses/UniversalPostProcessPass.cpp"
   "Pipelines/URP/RenderPasses/UniversalSSAOPass.h"  "Pipelines/URP/RenderPasses/UniversalSSAOPass.cpp"
   "Pipelines/URP/RenderPasses/UniversalDeferredLightingPass.h" "Pipelines/URP/RenderPasses/UniversalDeferredLightingPass.cpp"
   "Pipelines/URP/UniversalGlobalUniforms.h"   "Pipelines/URP/UniversalGlobalUniforms.cpp"
   "Pipelines/URP/UniversalRenderContext.h"    "Pipelines/URP/UniversalRenderContext.cpp"
   "Pipelines/URP/UniversalRenderPipeline.h"   "Pipelines/URP/UniversalRenderPipeline.cpp"

   # Misc
   "Misc/Defines.h"
   "Misc/Types.h"
   "Misc/ImguiManager.h"            "Misc/ImguiManager.cpp"
   "Misc/Utils.h"                   "Misc/Utils.cpp"       
   "Misc/IBLUtils.h"                "Misc/IBLUtils.cpp"
   "Misc/AssetFormat.h"             "Misc/AssetFormat.cpp"
   "Misc/AssetUtils.h"              "Misc/AssetUtils.cpp"
   "Misc/PhysicsHelper.h"           "Misc/PhysicsHelper.cpp"

   # Graphics
   "Graphics/Shader.h"              "Graphics/Shader.cpp"
   "Graphics/Program.h"             "Graphics/Program.cpp"
   "Graphics/Buffer.h"              "Graphics/Buffer.cpp" 
   "Graphics/VertexLayout.h"        "Graphics/VertexLayout.cpp"
   "Graphics/Vertex.h"              "Graphics/Vertex.cpp"
   "Graphics/UniformBuffer.h"       "Graphics/UniformBuffer.cpp"
   "Graphics/Uniform.h"             "Graphics/Uniform.cpp"
   "Graphics/Frustum.h"             "Graphics/Frustum.cpp"
   "Graphics/Geometry.h"            "Graphics/Geometry.cpp"
   "Graphics/CubeFramebuffer.h"     "Graphics/CubeFramebuffer.cpp"
   "Graphics/SkyLight.h"            "Graphics/SkyLight.cpp"
   "Graphics/ShadowMap.h"           "Graphics/ShadowMap.cpp"

   # Framebuffers
   "Framebuffers/Framebuffer.h"             "Framebuffers/Framebuffer.cpp"
   "Framebuffers/BRDFLookUpFramebuffer.h"   "Framebuffers/BRDFLookUpFramebuffer.cpp"
   "Framebuffers/GBufferFramebuffer.h"      "Framebuffers/GBufferFramebuffer.cpp"
   "Framebuffers/MSAAFramebuffer.h"         "Framebuffers/MSAAFramebuffer.cpp"
   "Framebuffers/PostProcessFramebuffer.h"  "Framebuffers/PostProcessFramebuffer.cpp"
   "Framebuffers/SSAOFramebuffer.h"         "Framebuffers/SSAOFramebuffer.cpp"

   # Resources
   "Resources/Resource.h"           "Resources/Resource.cpp"
   "Resources/AudioClip.h"          "Resources/AudioClip.cpp"
   "Resources/Image.h"              "Resources/Image.cpp"
   "Resources/Model.h"              "Resources/Model.cpp"
   "Resources/Animation.h"          "Resources/Animation.cpp"
   "Resources/AnimChannel.h"        "Resources/AnimChannel.cpp"
   "Resources/Pose.h"               "Resources/Pose.cpp"
   "Resources/Skeleton.h"           "Resources/Skeleton.cpp"
   "Resources/Texture.h"            "Resources/Texture.cpp" 
   "Resources/Mesh.h"               "Resources/Mesh.cpp"
   "Resources/StaticMesh.h"         "Resources/StaticMesh.cpp" 
   "Resources/SkinnedMesh.h"        "Resources/SkinnedMesh.cpp"
   "Resources/InstancedMesh.h"      "Resources/InstancedMesh.cpp"
   "Resources/ScreenMesh.h"         "Resources/ScreenMesh.cpp"
   "Resources/Material.h"           "Resources/Material.cpp"
   "Resources/CubeTexture.h"        "Resources/CubeTexture.cpp"
   "Resources/AnimController.h"    "Resources/AnimController.cpp"
   "Resources/AnimState.h"          "Resources/AnimState.cpp"

   # Components
   "Components/Component.h"         "Components/Component.cpp"
   "Components/Light.h"             "Components/Light.cpp"
   "Components/Script.h"            "Components/Script.cpp"
   "Components/PhysicsComponent.h"  "Components/PhysicsComponent.cpp"
   "Components/MeshRenderer.h"      "Components/MeshRenderer.cpp"

   "Components/Camera.h"            "Components/Camera.cpp"
   "Components/Transform.h"         "Components/Transform.cpp"
   "Components/DirectionalLight.h"  "Components/DirectionalLight.cpp"
   "Components/PointLight.h"        "Components/PointLight.cpp"
   "Components/SpotLight.h"         "Components/SpotLight.cpp"
   "Components/Animator.h"          "Components/Animator.cpp"
   "Components/AudioSource.h"       "Components/AudioSource.cpp"
   "Components/AudioListener.h"     "Components/AudioListener.cpp"
   "Components/MeshOutline.h"       "Components/MeshOutline.cpp"
   "Components/StaticMeshRenderer.h"   "Components/StaticMeshRenderer.cpp"
   "Components/SkinnedMeshRenderer.h"  "Components/SkinnedMeshRenderer.cpp"
   "Components/InstancedMeshRenderer.h" "Components/InstancedMeshRenderer.cpp"

   # Managers
   "Managers/TimeManager.h"         "Managers/TimeManager.cpp"
   "Managers/WindowManager.h"       "Managers/WindowManager.cpp"
   "Managers/InputManager.h"        "Managers/InputManager.cpp"           
   "Managers/ResourceManager.h"     "Managers/ResourceManager.cpp"
   "Managers/SceneManager.h"        "Managers/SceneManager.cpp"
   "Managers/RenderManager.h"       "Managers/RenderManager.cpp"
   "Managers/AudioManager.h"        "Managers/AudioManager.cpp"
   "Managers/PhysicsManager.h"      "Managers/PhysicsManager.cpp"

)

# 2. [PCH 설정] EnginePCH.h를 PCH로 사용합니다.
target_precompile_headers(Engine  PUBLIC EnginePCH.h)

# 3. [헤더 경로] 다른 프로젝트(Game)가 Engine의 헤더 파일을
#    찾을 수 있도록 현재 폴더를 PUBLIC include 경로로 지정합니다.
target_include_directories(Engine PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty
)

# 4. 외부 라이브러리를 여기에 링크
target_link_libraries(Engine PUBLIC
    glad::glad
    glfw
    glm::glm
    imgui::imgui
    spdlog::spdlog
    assimp::assimp
    KTX::ktx
    freetype  
    Jolt::Jolt
)

# 5. [컴파일 정의] Engine 타겟에 C++ 매크로를 정의합니다.
target_compile_definitions(Engine PUBLIC
    WINDOW_WIDTH=1200
    WINDOW_HEIGHT=800
    WINDOW_NAME="My3DGameFramework"
)