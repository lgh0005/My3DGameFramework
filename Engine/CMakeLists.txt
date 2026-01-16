find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(KTX CONFIG REQUIRED)
find_package(freetype REQUIRED)
find_package(Jolt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(recastnavigation CONFIG REQUIRED)

add_library(Engine STATIC

    # PCH
   "EnginePch.h"                    "EnginePch.cpp"

   # Core
   "Core/Application.h"             "Core/Application.cpp"
   "Core/Logger.h"                  "Core/Logger.cpp"
   "Core/ResourceLoader.h"          "Core/ResourceLoader.cpp"
   "Core/EngineConfigurator.h"    "Core/EngineConfigurator.cpp"

   # Scene
   "Scene/Scene.h"                   "Scene/Scene.cpp"
   "Scene/GameObjectRegistry.h"       "Scene/GameObjectRegistry.cpp"
   "Scene/ComponentRegistry.h"           "Scene/ComponentRegistry.cpp"      "Scene/ComponentRegistry.inl"
   "Scene/SceneRegistry.h"          "Scene/SceneRegistry.cpp"
   "Scene/SceneUtils.h"              "Scene/SceneUtils.cpp"

   # Object
   "Object/Object.h"                  "Object/Object.cpp"       
   "Object/GameObject.h"              "Object/GameObject.cpp"    "Object/GameObject.inl"
   "Object/Component.h"               "Object/Component.cpp"
   "Object/Prefab.h"                  "Object/Prefab.cpp"
   "Object/ScriptRegistry.h"          "Object/ScriptRegistry.cpp"

   # Common Render Passes
   "Pipelines/Common/CullingPass.h" "Pipelines/Common/CullingPass.cpp"
   "Pipelines/Common/ShadowPass.h"  "Pipelines/Common/ShadowPass.cpp"
   "Pipelines/Common/SkyboxPass.h"  "Pipelines/Common/SkyboxPass.cpp"
   "Pipelines/Common/UIScreenSpacePass.h"  "Pipelines/Common/UIScreenSpacePass.cpp"
   "Pipelines/Common/UIWorldSpacePass.h"  "Pipelines/Common/UIWorldSpacePass.cpp"
   "Pipelines/Common/UIScreenSpacePass.h" "Pipelines/Common/UIScreenSpacePass.cpp"
   "Pipelines/Common/JoltDebugGizmoPass.h"  "Pipelines/Common/JoltDebugGizmoPass.cpp"

   # Standard Render Pipeline
   "Pipelines/SRP/RenderPasses/StandardPostProcessPass.h" "Pipelines/SRP/RenderPasses/StandardPostProcessPass.cpp"
   "Pipelines/SRP/RenderPasses/StandardGeometryPass.h"  "Pipelines/SRP/RenderPasses/StandardGeometryPass.cpp"
   "Pipelines/SRP/RenderPasses/StandardDeferredLightingPass.h"  "Pipelines/SRP/RenderPasses/StandardDeferredLightingPass.cpp"
   "Pipelines/SRP/RenderPasses/StandardSSAOPass.h"    "Pipelines/SRP/RenderPasses/StandardSSAOPass.cpp"
   "Pipelines/SRP/RenderPasses/StandardPostProcessPass.h"  "Pipelines/SRP/RenderPasses/StandardPostProcessPass.cpp"
   "Pipelines/SRP/StandardGlobalUniforms.h"    "Pipelines/SRP/StandardGlobalUniforms.cpp"
   "Pipelines/SRP/StandardRenderContext.h"     "Pipelines/SRP/StandardRenderContext.cpp"
   "Pipelines/SRP/StandardRenderPipeline.h"    "Pipelines/SRP/StandardRenderPipeline.cpp"
   "Pipelines/SRP/RenderPasses/StandardOutlinePass.h" "Pipelines/SRP/RenderPasses/StandardOutlinePass.cpp"

   # Universal Render Pipeline
   #"Pipelines/URP/RenderPasses/UniversalGeometryPass.h" "Pipelines/URP/RenderPasses/UniversalGeometryPass.cpp"
   #"Pipelines/URP/RenderPasses/UniversalPostProcessPass.h"  "Pipelines/URP/RenderPasses/UniversalPostProcessPass.cpp"
   #"Pipelines/URP/RenderPasses/UniversalSSAOPass.h"  "Pipelines/URP/RenderPasses/UniversalSSAOPass.cpp"
   #"Pipelines/URP/RenderPasses/UniversalDeferredLightingPass.h" "Pipelines/URP/RenderPasses/UniversalDeferredLightingPass.cpp"
   #"Pipelines/URP/UniversalGlobalUniforms.h"   "Pipelines/URP/UniversalGlobalUniforms.cpp"
   #"Pipelines/URP/UniversalRenderContext.h"    "Pipelines/URP/UniversalRenderContext.cpp"
   #"Pipelines/URP/UniversalRenderPipeline.h"   "Pipelines/URP/UniversalRenderPipeline.cpp"
   #"Pipelines/URP/RenderPasses/UniversalOutlinePass.h"   "Pipelines/URP/RenderPasses/UniversalOutlinePass.cpp"

   # Misc
   "Misc/Defines.h"                 
   "Misc/Types.h"
   "Misc/Enums.h"

   "Misc/ImguiManager.h"            "Misc/ImguiManager.cpp"
   "Misc/Utils.h"                   "Misc/Utils.cpp"       
   "Misc/AssetFormat.h"             "Misc/AssetFormat.cpp"
   "Misc/AssetUtils.h"              "Misc/AssetUtils.cpp"
   "Misc/ResourceJsonFormat.h"      "Misc/ResourceJsonFormat.cpp"

   # Physics
   "Physics/BPLayerInterface.h"         "Physics/BPLayerInterface.cpp"
   "Physics/PhysicsConfig.h"            "Physics/PhysicsConfig.cpp"
   "Physics/PhysicsFilters.h"           "Physics/PhysicsFilters.cpp"
   "Physics/PhysicsCallbacks.h"         "Physics/PhysicsCallbacks.cpp"
   "Physics/PhysicsContactListener.h"    "Physics/PhysicsContactListener.cpp"
   "Physics/PhysicsBodyActivationListener.h"    "Physics/PhysicsBodyActivationListener.cpp"
   "Physics/JoltDebugRenderer.h"        "Physics/JoltDebugRenderer.cpp"
   "Physics/JoltGizmo.h"                "Physics/JoltGizmo.cpp"
   "Physics/GizmoCommand.h"         "Physics/GizmoCommand.cpp"
   "Physics/RigidbodyConstraint.h"      "Physics/RigidbodyConstraint.cpp"

   # Graphics
   "Graphics/Shader.h"              "Graphics/Shader.cpp"
   "Graphics/Buffer.h"              "Graphics/Buffer.cpp" 
   "Graphics/VertexLayout.h"        "Graphics/VertexLayout.cpp"
   "Graphics/Vertex.h"              "Graphics/Vertex.cpp"
   "Graphics/UniformBuffer.h"       "Graphics/UniformBuffer.cpp"
   "Graphics/Uniform.h"             "Graphics/Uniform.cpp"
   "Graphics/IBLUtils.h"            "Graphics/IBLUtils.cpp"
   "Graphics/Frustum.h"             "Graphics/Frustum.cpp"
   "Graphics/Geometry.h"            "Graphics/Geometry.cpp"
   "Graphics/CubeFramebuffer.h"     "Graphics/CubeFramebuffer.cpp"
   "Graphics/SkyLight.h"            "Graphics/SkyLight.cpp"
   "Graphics/ShadowMap.h"           "Graphics/ShadowMap.cpp"
   "Graphics/Ray.h"                 "Graphics/Ray.cpp"
   "Graphics/RaycastHit.h"          "Graphics/RaycastHit.cpp"
   "Graphics/Renderer.h"                "Graphics/Renderer.cpp"
   "Graphics/RenderPass.h"              "Graphics/RenderPass.cpp"
   "Graphics/RenderPipeline.h"          "Graphics/RenderPipeline.cpp"
   "Graphics/RenderContext.h"           "Graphics/RenderContext.cpp"
   "Graphics/GlobalUniforms.h"          "Graphics/GlobalUniforms.cpp"

   # Framebuffers
   "Framebuffers/Framebuffer.h"             "Framebuffers/Framebuffer.cpp"
   "Framebuffers/BRDFLookUpFramebuffer.h"   "Framebuffers/BRDFLookUpFramebuffer.cpp"
   "Framebuffers/GBufferFramebuffer.h"      "Framebuffers/GBufferFramebuffer.cpp"
   "Framebuffers/MSAAFramebuffer.h"         "Framebuffers/MSAAFramebuffer.cpp"
   "Framebuffers/PostProcessFramebuffer.h"  "Framebuffers/PostProcessFramebuffer.cpp"
   "Framebuffers/SSAOFramebuffer.h"         "Framebuffers/SSAOFramebuffer.cpp"
   "Framebuffers/OutlineFramebuffer.h"      "Framebuffers/OutlineFramebuffer.cpp"

   # Resources
   "Resources/Resource.h"           "Resources/Resource.cpp"
   "Resources/Program.h"            "Resources/Program.cpp"
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
   "Resources/DebugMesh.h"          "Resources/DebugMesh.cpp"
   "Resources/UIMesh.h"             "Resources/UIMesh.cpp"
   "Resources/Material.h"           "Resources/Material.cpp"
   "Resources/CubeTexture.h"        "Resources/CubeTexture.cpp"
   "Resources/AnimController.h"     "Resources/AnimController.cpp"
   "Resources/AnimState.h"          "Resources/AnimState.cpp"
   "Resources/Font.h"               "Resources/Font.cpp"
   "Resources/EnvironmentMap.h"    "Resources/EnvironmentMap.cpp"

   # Components
   "Components/Light.h"             "Components/Light.cpp"
   "Components/Script.h"            "Components/Script.cpp"
   "Components/PhysicsComponent.h"  "Components/PhysicsComponent.cpp"
   "Components/MeshRenderer.h"      "Components/MeshRenderer.cpp"
   "Components/Collider.h"          "Components/Collider.cpp"

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
   "Components/BoxCollider.h"           "Components/BoxCollider.cpp"
   "Components/SphereCollider.h"        "Components/SphereCollider.cpp"
   "Components/CapsuleCollider.h"       "Components/CapsuleCollider.cpp"
   "Components/Rigidbody.h"             "Components/Rigidbody.cpp"
   "Components/Terrain.h"               "Components/Terrain.cpp"

   # Navigation Mesh
   "NavMesh/NavigationManager.h"        "NavMesh/NavigationManager.cpp"
   "NavMesh/NavMeshFunctors.h"          "NavMesh/NavMeshFunctors.cpp"

   # UIs
   "UIs/UIRenderer.h"               "UIs/UIRenderer.cpp"
   "UIs/UICanvas.h"                 "UIs/UICanvas.cpp"
   "UIs/UIImage.h"                  "UIs/UIImage.cpp"
   "UIs/UIText.h"                   "UIs/UIText.cpp"

   # Managers
   "Managers/TimeManager.h"         "Managers/TimeManager.cpp"
   "Managers/WindowManager.h"       "Managers/WindowManager.cpp"
   "Managers/InputManager.h"        "Managers/InputManager.cpp"          "Managers/InputManager.inl"     
   "Managers/ResourceManager.h"     "Managers/ResourceManager.cpp"       "Managers/ResourceManager.inl"
   "Managers/SceneManager.h"        "Managers/SceneManager.cpp"           "Managers/SceneManager.inl"
   "Managers/RenderManager.h"       "Managers/RenderManager.cpp"         "Managers/RenderManager.inl"
   "Managers/AudioManager.h"        "Managers/AudioManager.cpp"
   "Managers/PhysicsManager.h"      "Managers/PhysicsManager.cpp"
   "Managers/UIManager.h"           "Managers/UIManager.cpp"
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
    nlohmann_json::nlohmann_json
    RecastNavigation::Recast
    RecastNavigation::Detour
    RecastNavigation::DetourCrowd
    RecastNavigation::DebugUtils
    RecastNavigation::DetourTileCache
)

# 5. [컴파일 정의] Engine 타겟에 C++ 매크로를 정의합니다.
target_compile_definitions(Engine PUBLIC
    WINDOW_WIDTH=1200
    WINDOW_HEIGHT=800
    WINDOW_NAME="My3DGameFramework"
)