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
   "Pipelines/Common/SSAOPass.h"    "Pipelines/Common/SSAOPass.cpp"
   "Pipelines/Common/PostProcessPass.h"     "Pipelines/Common/PostProcessPass.cpp"

   # Standard Render Pipeline
   "Pipelines/SRP/RenderPasses/StandardGeometryPass.h"  "Pipelines/SRP/RenderPasses/StandardGeometryPass.cpp"
   "Pipelines/SRP/RenderPasses/StandardDeferredLightingPass.h"  "Pipelines/SRP/RenderPasses/StandardDeferredLightingPass.cpp"
   "Pipelines/SRP/StandardGlobalUniforms.h"    "Pipelines/SRP/StandardGlobalUniforms.cpp"
   "Pipelines/SRP/StandardRenderPipeline.h"    "Pipelines/SRP/StandardRenderPipeline.cpp"
   
   # Universal Render Pipeline
   "Pipelines/URP/RenderPasses/UniversalGeometryPass.h" "Pipelines/URP/RenderPasses/UniversalGeometryPass.cpp"
   "Pipelines/URP/RenderPasses/UniversalDeferredLightingPass.h" "Pipelines/URP/RenderPasses/UniversalDeferredLightingPass.cpp"
   "Pipelines/URP/UniversalGlobalUniforms.h"   "Pipelines/URP/UniversalGlobalUniforms.cpp"
   "Pipelines/URP/UniversalRenderPipeline.h"   "Pipelines/URP/UniversalRenderPipeline.cpp"

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
   "Graphics/ShadowMap.h"           "Graphics/ShadowMap.cpp"
   "Graphics/Ray.h"                 "Graphics/Ray.cpp"
   "Graphics/RaycastHit.h"          "Graphics/RaycastHit.cpp"
   "Graphics/Renderer.h"                "Graphics/Renderer.cpp"
   "Graphics/RenderPass.h"              "Graphics/RenderPass.cpp"
   "Graphics/RenderPipeline.h"          "Graphics/RenderPipeline.cpp"
   "Graphics/RenderContext.h"           "Graphics/RenderContext.cpp"
   "Graphics/GlobalUniforms.h"          "Graphics/GlobalUniforms.cpp"
   "Graphics/Velocitybuffer.h"      "Graphics/Velocitybuffer.cpp"

   # Post-processing effects
   "Pipelines/PostProcessing/PostProcessEffect.h"    "Pipelines/PostProcessing/PostProcessEffect.cpp"
   "Pipelines/PostProcessing/World/MotionBlurEffect.h"    "Pipelines/PostProcessing/World/MotionBlurEffect.cpp"
   "Pipelines/PostProcessing/World/OutlineEffect.h"       "Pipelines/PostProcessing/World/OutlineEffect.cpp"
   "Pipelines/PostProcessing/World/WorldSpaceEffects.h" "Pipelines/PostProcessing/World/WorldSpaceEffects.cpp"

   "Pipelines/PostProcessing/Bloom/DOFEffect.h"         "Pipelines/PostProcessing/Bloom/DOFEffect.cpp"
   "Pipelines/PostProcessing/Bloom/LensFlareEffect.h"   "Pipelines/PostProcessing/Bloom/LensFlareEffect.cpp"
   "Pipelines/PostProcessing/Bloom/GaussianBloomEffect.h"  "Pipelines/PostProcessing/Bloom/GaussianBloomEffect.cpp"
   "Pipelines/PostProcessing/Bloom/KawaseBloomEffect.h" "Pipelines/PostProcessing/Bloom/KawaseBloomEffect.cpp"

   "Pipelines/PostProcessing/Display/DisplayMappingEffects.h"  "Pipelines/PostProcessing/Display/DisplayMappingEffects.cpp"
   
   # Framebuffers
   "Graphics/Framebuffers/FramebufferBase.h"       "Graphics/Framebuffers/FramebufferBase.cpp"
   "Graphics/Framebuffers/Framebuffer.h"             "Graphics/Framebuffers/Framebuffer.cpp"
   "Graphics/Framebuffers/CubeFramebuffer.h"         "Graphics/Framebuffers/CubeFramebuffer.cpp"
   "Graphics/Framebuffers/BRDFLookUpFramebuffer.h"   "Graphics/Framebuffers/BRDFLookUpFramebuffer.cpp"
   "Graphics/Framebuffers/GBufferFramebuffer.h"      "Graphics/Framebuffers/GBufferFramebuffer.cpp"
   "Graphics/Framebuffers/MSAAFramebuffer.h"         "Graphics/Framebuffers/MSAAFramebuffer.cpp"
   "Graphics/Framebuffers/PostProcessFramebuffer.h"  "Graphics/Framebuffers/PostProcessFramebuffer.cpp"
   "Graphics/Framebuffers/SSAOFramebuffer.h"         "Graphics/Framebuffers/SSAOFramebuffer.cpp"
   "Graphics/Framebuffers/OutlineFramebuffer.h"      "Graphics/Framebuffers/OutlineFramebuffer.cpp"
   "Graphics/Framebuffers/OutlineFramebuffer.h"      "Graphics/Framebuffers/OutlineFramebuffer.cpp"
   "Graphics/Framebuffers/EffectFramebuffer.h"       "Graphics/Framebuffers/EffectFramebuffer.cpp"

   # Resources
   "Resources/Resource.h"           "Resources/Resource.cpp"
   "Resources/Program.h"            "Resources/Program.cpp"
   "Resources/AudioClip.h"          "Resources/AudioClip.cpp"
   "Resources/Image.h"              "Resources/Image.cpp"
   "Resources/Model.h"              "Resources/Model.cpp"
   "Resources/Material.h"           "Resources/Material.cpp"
   "Resources/Font.h"               "Resources/Font.cpp"
   "Resources/EnvironmentMap.h"     "Resources/EnvironmentMap.cpp"

   "Resources/Animations/Animation.h"          "Resources/Animations/Animation.cpp"
   "Resources/Animations/AnimChannel.h"        "Resources/Animations/AnimChannel.cpp"
   "Resources/Animations/Pose.h"               "Resources/Animations/Pose.cpp"
   "Resources/Animations/Skeleton.h"           "Resources/Animations/Skeleton.cpp"
   "Resources/Animations/AnimController.h"    "Resources/Animations/AnimController.cpp"
   "Resources/Animations/AnimState.h"          "Resources/Animations/AnimState.cpp"

   "Resources/Textures/TextureBase.h"        "Resources/Textures/TextureBase.cpp" 
   "Resources/Textures/Texture.h"            "Resources/Textures/Texture.cpp" 
   "Resources/Textures/TextureUtils.h"       "Resources/Textures/TextureUtils.cpp" 
   "Resources/Textures/CubeTexture.h"        "Resources/Textures/CubeTexture.cpp"

   "Resources/Meshes/Mesh.h"                 "Resources/Meshes/Mesh.cpp"
   "Resources/Meshes/StaticMesh.h"           "Resources/Meshes/StaticMesh.cpp" 
   "Resources/Meshes/SkinnedMesh.h"          "Resources/Meshes/SkinnedMesh.cpp"
   "Resources/Meshes/InstancedMesh.h"        "Resources/Meshes/InstancedMesh.cpp"
   "Resources/Meshes/ScreenMesh.h"           "Resources/Meshes/ScreenMesh.cpp"
   "Resources/Meshes/DebugMesh.h"            "Resources/Meshes/DebugMesh.cpp"
   "Resources/Meshes/UIMesh.h"               "Resources/Meshes/UIMesh.cpp"
   

   # Components
   "Components/Light.h"             "Components/Light.cpp"
   "Components/Script.h"            "Components/Script.cpp"
   "Components/PhysicsComponent.h"  "Components/PhysicsComponent.cpp"
   "Components/MeshRenderer.h"      "Components/MeshRenderer.cpp"
   "Components/Collider.h"          "Components/Collider.cpp"

   "Components/SkyLight.h"          "Components/SkyLight.cpp"
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