# 0. 라이브러리 이름
set(MODULE_NAME Components) 
set(LIB_NAME    ${MODULE_NAME}Lib)
set(TARGET_NAME Engine::${MODULE_NAME})

# 1. 소스 파일 목록 정의
set(SOURCES

    # 0. Module
    "ComponentModule.h"          "ComponentModule.cpp"  

    # 1. Descriptors

    # 2. Core
    "Core/Transform.h"          "Core/Transform.cpp"

    # 3. Animator
    "Animator/Animator.h"          "Animator/Animator.cpp"

    # 4. Audios
    "Audios/AudioListener.h"          "Audios/AudioListener.cpp"
    "Audios/AudioSource.h"          "Audios/AudioSource.cpp"

    # 5. Camera
    "Camera/Camera.h"          "Camera/Camera.cpp"

    # 6. Colliders
    "Colliders/Collider.h"          "Colliders/Collider.cpp"
    "Colliders/BoxCollider.h"          "Colliders/BoxCollider.cpp"
    "Colliders/SphereCollider.h"          "Colliders/SphereCollider.cpp"
    "Colliders/CapsuleCollider.h"          "Colliders/CapsuleCollider.cpp"

    # 7. Lights
    "Lights/Light.h"                "Lights/Light.cpp"
    "Lights/DirectionalLight.h"                "Lights/DirectionalLight.cpp"
    "Lights/SpotLight.h"                "Lights/SpotLight.cpp"
    "Lights/PointLight.h"                "Lights/PointLight.cpp"
    "Lights/SkyLight.h"                "Lights/SkyLight.cpp"

    # 8. MeshRenderers
    "MeshRenderers/MeshRenderer.h"                "MeshRenderers/MeshRenderer.cpp"
    "MeshRenderers/StaticMeshRenderer.h"          "MeshRenderers/StaticMeshRenderer.cpp"
    "MeshRenderers/SkinnedMeshRenderer.h"         "MeshRenderers/SkinnedMeshRenderer.cpp"
    "MeshRenderers/InstancedMeshRenderer.h"       "MeshRenderers/InstancedMeshRenderer.cpp"
    "MeshRenderers/MeshOutline.h"                "MeshRenderers/MeshOutline.cpp"

    # 9. Rigidbody
    "Rigidbody/PhysicsComponent.h"                "Rigidbody/PhysicsComponent.cpp"
    "Rigidbody/Rigidbody.h"                       "Rigidbody/Rigidbody.cpp"
    
    # 10. Script
    "Script/Script.h"             "Script/Script.cpp"             "Script/Script.cpp"

    # 11. UIs
    "UIs/UICanvas.h"                       "UIs/UICanvas.cpp"
    "UIs/UIImage.h"                       "UIs/UIImage.cpp"
    "UIs/UIRenderer.h"                       "UIs/UIRenderer.cpp"
    "UIs/UIText.h"                       "UIs/UIText.cpp"
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
# 옵션 A: 이 라이브러리만의 개별 PCH를 사용할 경우
target_precompile_headers(${LIB_NAME} PRIVATE "ComponentPch.h")

# 옵션 B: CoreLib에 정의된 PCH를 재사용할 경우 (빌드 속도 최적화)
# target_precompile_headers(${LIB_NAME} REUSE_FROM CoreLib)