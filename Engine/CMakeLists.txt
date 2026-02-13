# 소스 파일 목록
set(ENGINE_SOURCES

    # PCH
   "EnginePch.h"                    "EnginePch.cpp"

   # Core
   "Core/Application.h"             "Core/Application.cpp"

   # Parsers
   "Parsers/Parser.h"               "Parsers/Parser.cpp"
   "Parsers/YamlParser.h"           "Parsers/YamlParser.cpp"
   "Parsers/JsonParser.h"           "Parsers/JsonParser.cpp"

   "Parsers/EngineParsers/MaterialYamlParser.h"           "Parsers/EngineParsers/MaterialYamlParser.cpp"
   "Parsers/EngineParsers/SceneYamlParser.h"           "Parsers/EngineParsers/SceneYamlParser.cpp"

   # Debug
   "Debug/Logger.h"                  "Debug/Logger.cpp"

   # Entity
   "Entity/Chunk.h"                 "Entity/Chunk.cpp"
   "Entity/Archetype.h"             "Entity/Archetype.cpp"

   # Scene
   "Scene/Scene.h"                   "Scene/Scene.cpp"
   "Scene/GameObjectRegistry.h"      "Scene/GameObjectRegistry.cpp"
   "Scene/ComponentRegistry.h"       "Scene/ComponentRegistry.cpp"      "Scene/ComponentRegistry.inl"
   "Scene/SceneRegistry.h"           "Scene/SceneRegistry.cpp"
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
   "Misc/Constants.h"

   "Misc/ImguiManager.h"            "Misc/ImguiManager.cpp"
   "Misc/Utils.h"                   "Misc/Utils.cpp"       
   "Misc/AssetFormat.h"             "Misc/AssetFormat.cpp"
   "Misc/AssetUtils.h"              "Misc/AssetUtils.cpp"
   "Misc/ResourceJsonFormat.h"      "Misc/ResourceJsonFormat.cpp"
   "Misc/ResourceFormat.h"          "Misc/ResourceFormat.cpp" 

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

   # Instancing
   "Instancing/InstanceBuffer.h"    "Instancing/InstanceBuffer.cpp"
   "Instancing/AnimBuffer.h"      "Instancing/AnimBuffer.cpp"
   "Instancing/InstanceProperty.h"      "Instancing/InstanceProperty.cpp"
   "Instancing/RenderBatch.h"       "Instancing/RenderBatch.cpp"
   "Instancing/RenderQueue.h"      "Instancing/RenderQueue.cpp"

   # Graphics
   "Graphics/Shader.h"              "Graphics/Shader.cpp"
   "Graphics/IBLUtils.h"            "Graphics/IBLUtils.cpp"
   "Graphics/ShadowMap.h"           "Graphics/ShadowMap.cpp"
   "Graphics/Velocitybuffer.h"      "Graphics/Velocitybuffer.cpp"

   "Graphics/Raycasting/Ray.h"                 "Graphics/Raycasting/Ray.cpp"
   "Graphics/Raycasting/RaycastHit.h"          "Graphics/Raycasting/RaycastHit.cpp"

    "Graphics/Geometry/Frustum.h"             "Graphics/Geometry/Frustum.cpp"
   "Graphics/Geometry/CullingPlane.h"           "Graphics/Geometry/CullingPlane.cpp"
   "Graphics/Geometry/RenderBounds.h"           "Graphics/Geometry/RenderBounds.cpp"
   "Graphics/Geometry/GeometryGenerator.h"   "Graphics/Geometry/GeometryGenerator.cpp"

   "Graphics/Rendering/Renderer.h"                "Graphics/Rendering/Renderer.cpp"
   "Graphics/Rendering/RenderPass.h"              "Graphics/Rendering/RenderPass.cpp"
   "Graphics/Rendering/RenderPipeline.h"          "Graphics/Rendering/RenderPipeline.cpp"
   "Graphics/Rendering/RenderContext.h"           "Graphics/Rendering/RenderContext.cpp"
   "Graphics/Rendering/GlobalUniforms.h"          "Graphics/Rendering/GlobalUniforms.cpp"

   "Graphics/Buffers/Buffer.h"               "Graphics/Buffers/Buffer.cpp" 
   "Graphics/Buffers/VertexBuffer.h"         "Graphics/Buffers/VertexBuffer.cpp" 
   "Graphics/Buffers/IndexBuffer.h"         "Graphics/Buffers/IndexBuffer.cpp" 
   "Graphics/Buffers/UniformBuffer.h"       "Graphics/Buffers/UniformBuffer.cpp"
   "Graphics/Buffers/ShaderStorageBuffer.h" "Graphics/Buffers/ShaderStorageBuffer.cpp"

   "Graphics/Layouts/VertexLayout.h"        "Graphics/Layouts/VertexLayout.cpp"
   "Graphics/Layouts/Vertex.h"              "Graphics/Layouts/Vertex.cpp"
   "Graphics/Layouts/Uniform.h"             "Graphics/Layouts/Uniform.cpp"

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
   "Resources/ResourceDesc.h"       "Resources/ResourceDesc.cpp"
   "Resources/AudioClip.h"          "Resources/AudioClip.cpp"
   "Resources/Image.h"              "Resources/Image.cpp"
   "Resources/Model.h"              "Resources/Model.cpp"
   "Resources/Material.h"           "Resources/Material.cpp"
   "Resources/Font.h"               "Resources/Font.cpp"
   "Resources/EnvironmentMap.h"     "Resources/EnvironmentMap.cpp"

   "Resources/Programs/Program.h"            "Resources/Programs/Program.cpp"
   "Resources/Programs/GraphicsProgram.h" "Resources/Programs/GraphicsProgram.cpp" 
   "Resources/Programs/ComputeProgram.h" "Resources/Programs/ComputeProgram.cpp"

   "Resources/Animations/Animation.h"          "Resources/Animations/Animation.cpp"
   "Resources/Animations/AnimClip.h"            "Resources/Animations/AnimClip.cpp"
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
   "Managers/EntityManager.h"           "Managers/EntityManager.cpp"
   "Managers/InstanceManager.h"      "Managers/InstanceManager.cpp"
   "Managers/FileManager.h"         "Managers/FileManager.cpp"          "Managers/FileManager.inl"
   "Managers/LuaManager.h"      "Managers/LuaManager.cpp"
)

# Visual Studio 필터 자동 생성
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_SOURCES})

# 라이브러리 생성
add_library(Engine STATIC ${ENGINE_SOURCES})

# Include 경로 및 PCH 설정
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 미리 컴파일된 헤더 적용
target_precompile_headers(Engine PUBLIC EnginePch.h)

# 라이브러리 링크 (Dependencies)
target_link_libraries(Engine PUBLIC
    glad::glad
    glfw
    glm::glm
    imgui::imgui
    spdlog::spdlog
    assimp::assimp
    KTX::ktx
    Freetype::Freetype
    Jolt::Jolt
    nlohmann_json::nlohmann_json
    yaml-cpp::yaml-cpp
    lua
    sol2::sol2

    RecastNavigation::Recast
    RecastNavigation::Detour
    RecastNavigation::DetourCrowd
    RecastNavigation::DetourTileCache
    RecastNavigation::DebugUtils
)

# 컴파일 정의
target_compile_definitions(Engine PUBLIC
    WINDOW_WIDTH=1200
    WINDOW_HEIGHT=800
    WINDOW_NAME="My3DGameFramework"
)

# 리소스 배포 (Resource Deployment)
add_custom_command(TARGET Engine POST_BUILD
    COMMENT "Deploying Engine Centralized Content to Bin..."

    # 1. Config (Engine)
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Config"
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${OUTPUT_DIR}/Config/Engine"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Config"
            "${OUTPUT_DIR}/Config/Engine"

    # 2. Engine boot config (root)
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Config/EngineConfig.json"
            "${OUTPUT_DIR}/Config/EngineConfig.json"

    # 3. Assets (Engine)
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/Assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/BuiltIn"
            "${OUTPUT_DIR}/Assets/BuiltIn"
)
