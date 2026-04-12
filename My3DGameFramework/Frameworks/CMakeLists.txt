# 0. 라이브러리 이름
set(MODULE_NAME Frameworks) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES
    
    # pch
    "FrameworkPch.cpp"

    # Managers
    # "Managers/ResourceManager.h"    "Managers/ResourceManager.cpp"  "Managers/ResourceManager.inl"

    # Components
    # "Components/Animation/Animator.h"       "Components/Animation/Animator.cpp"
    # "Components/Audios/AudioListener.h"     "Components/Audios/AudioListener.cpp"
    # "Components/Audios/AudioSource.h"       "Components/Audios/AudioSource.cpp"
    # "Components/Colliders/Collider.h"     "Components/Colliders/Collider.cpp"
    # "Components/Colliders/BoxCollider.h"        "Components/Colliders/BoxCollider.cpp"
    # "Components/Colliders/SphereCollider.h"     "Components/Colliders/SphereCollider.cpp"
    # "Components/Colliders/CapsuleCollider.h"        "Components/Colliders/CapsuleCollider.cpp"
    # "Components/Core/Camera.h"        "Components/Core/Camera.cpp"
    # "Components/Core/Transform.h"        "Components/Core/Transform.cpp"
    # "Components/Lights/Light.h"        "Components/Lights/Light.cpp"
    # "Components/Lights/SkyLight.h"        "Components/Lights/SkyLight.cpp"
    # "Components/Lights/PointLight.h"        "Components/Lights/PointLight.cpp"
    # "Components/Lights/SpotLight.h"        "Components/Lights/SpotLight.cpp"
    # "Components/Lights/DirectionalLight.h"        "Components/Lights/DirectionalLight.cpp"
    # "Components/MeshRenderers/MeshRenderer.h"           "Components/MeshRenderers/MeshRenderer.cpp" 
    # "Components/MeshRenderers/StaticMeshRenderer.h"    "Components/MeshRenderers/StaticMeshRenderer.cpp" 
    # "Components/MeshRenderers/SkinnedMeshRenderer.h"   "Components/MeshRenderers/SkinnedMeshRenderer.cpp" 
    # "Components/MeshRenderers/MeshOutline.h"           "Components/MeshRenderers/MeshOutline.cpp" 
    # "Components/Physics/PhysicsComponent.h"            "Components/Physics/PhysicsComponent.cpp" 
    # "Components/Physics/Rigidbody.h"                  "Components/Physics/Rigidbody.cpp" 
    # "Components/Script/Script.h"                  "Components/Script/Script.cpp" 

    # Mechanics
    # "Mechanics/Animation/AnimChannel.h"             "Mechanics/Animation/AnimChannel.cpp"
    # "Mechanics/Animation/AnimClip.h"             "Mechanics/Animation/AnimClip.cpp"
    # "Mechanics/Animation/AnimController.h"             "Mechanics/Animation/AnimController.cpp"
    # "Mechanics/Animation/AnimState.h"             "Mechanics/Animation/AnimState.cpp"
    # "Mechanics/Animation/Pose.h"             "Mechanics/Animation/Pose.cpp"
    # "Mechanics/Animation/Skeleton.h"             "Mechanics/Animation/Skeleton.cpp"
    # "Mechanics/Camera/Velocitybuffer.h"             "Mechanics/Camera/Velocitybuffer.cpp"

    # Assets
    # "Assets/Animation.h"             "Assets/Animation.cpp"
    # "Assets/AudioClip.h"             "Assets/AudioClip.cpp"
    # "Assets/Image.h"             "Assets/Image.cpp"
    # "Assets/Model.h"             "Assets/Model.cpp"

    # Resources
    # "Resources/Material.h"             "Resources/Material.cpp"
    # "Resources/EnvironmentMap.h"             "Resources/EnvironmentMap.cpp"

    # Rendering
    # "Rendering/Renderer.h"          "Rendering/Renderer.cpp"
    # "Rendering/GlobalUniforms.h"          "Rendering/GlobalUniforms.cpp"
    # "Rendering/RenderContext.h"          "Rendering/RenderContext.cpp"
    # "Rendering/RenderPass.h"          "Rendering/RenderPass.cpp"
    # "Rendering/RenderPipeline.h"          "Rendering/RenderPipeline.cpp"
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
    # Engine::Core
)

# 6. PCH (Precompiled Header) 설정
target_precompile_headers(${LIB_NAME} PRIVATE "FrameworkPch.h")