# 0. 라이브러리 이름
set(MODULE_NAME Scene) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # Components
    "Components/Animator.h"              "Components/Animator.cpp"
    "Components/AudioListener.h"         "Components/AudioListener.cpp"
    "Components/AudioSource.h"           "Components/AudioSource.cpp"
    "Components/BoxCollider.h"           "Components/BoxCollider.cpp"
    "Components/Camera.h"                "Components/Camera.cpp"
    "Components/CapsuleCollider.h"       "Components/CapsuleCollider.cpp"
    "Components/Collider.h"              "Components/Collider.cpp"
    "Components/DirectionalLight.h"      "Components/DirectionalLight.cpp"
    "Components/InstancedMeshRenderer.h" "Components/InstancedMeshRenderer.cpp"
    "Components/Light.h"                 "Components/Light.cpp"
    "Components/MeshOutline.h"           "Components/MeshOutline.cpp"
    "Components/MeshRenderer.h"          "Components/MeshRenderer.cpp"
    "Components/PhysicsComponent.h"      "Components/PhysicsComponent.cpp"
    "Components/PointLight.h"            "Components/PointLight.cpp"
    "Components/Rigidbody.h"             "Components/Rigidbody.cpp"
    "Components/Script.h"                "Components/Script.cpp"   "Components/Script.inl"
    "Components/SkinnedMeshRenderer.h"   "Components/SkinnedMeshRenderer.cpp"
    "Components/SkyLight.h"              "Components/SkyLight.cpp"
    "Components/SphereCollider.h"        "Components/SphereCollider.cpp"
    "Components/SpotLight.h"             "Components/SpotLight.cpp"
    "Components/StaticMeshRenderer.h"    "Components/StaticMeshRenderer.cpp"
    "Components/Terrain.h"               "Components/Terrain.cpp"
    "Components/Transform.h"             "Components/Transform.cpp"
    "Components/UICanvas.h"              "Components/UICanvas.cpp"
    "Components/UIImage.h"               "Components/UIImage.cpp"
    "Components/UIText.h"                "Components/UIText.cpp"

    # Entity
    "Entity/Archetype.h"            "Entity/Archetype.cpp"
    "Entity/Chunk.h"            "Entity/Chunk.cpp"

    # Managers
    "Managers/EntityManager.h"      "Managers/EntityManager.cpp"
    "Managers/ResourceManager.h"      "Managers/ResourceManager.cpp"    "Managers/ResourceManager.inl"
    "Managers/SceneManager.h"      "Managers/SceneManager.cpp"    "Managers/SceneManager.inl"

    # Object
    "Object/Component.h"                "Object/Component.cpp"
    "Object/GameObject.h"               "Object/GameObject.cpp"  "Object/GameObject.inl"
    "Object/Scene.h"                    "Object/Scene.cpp"                    "Object/Scene.inl"
    "Object/Component.h"                "Object/Component.cpp"
    "Object/SceneUtils.h"               "Object/SceneUtils.cpp"

    # Scene
    "Registries/ComponentRegistry.h"     "Registries/ComponentRegistry.cpp"        "Registries/ComponentRegistry.inl"
    "Registries/GameObjectRegistry.h"    "Registries/GameObjectRegistry.cpp"
    "Registries/SceneRegistry.h"         "Registries/SceneRegistry.cpp"

    # Utilities
    "Utilities/ResourceDesc.h"           "Utilities/ResourceDesc.cpp")

# 2. 빌드 타겟 생성 및 별칭 등록
add_library(${LIB_NAME} STATIC ${SOURCES})
add_library(${TARGET_NAME} ALIAS ${LIB_NAME})

# 3. Visual Studio 필터 설정
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# 4. Include 경로 설정
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# 5. 의존성 링크
target_link_libraries(${LIB_NAME} PUBLIC
    
)

# 6. PCH (Precompiled Header) 설정
# 옵션 A: 이 라이브러리만의 개별 PCH를 사용할 경우
# target_precompile_headers(${LIB_NAME} PRIVATE "AiPch.h")

# 옵션 B: CoreLib에 정의된 PCH를 재사용할 경우 (빌드 속도 최적화)
# target_precompile_headers(${LIB_NAME} REUSE_FROM CoreLib)