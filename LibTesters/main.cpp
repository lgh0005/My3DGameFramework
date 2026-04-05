#include "pch.h"
#include <glad/glad.h>

#include "Bootstrapper/Bootstrapper.h"
#include "Debug/MemoryProfiler.h"
#include "Debug/VRAMProfiler.h"
#include "Pointer/RefCount.h"

#include "Managers/TimeManager.h"
#include "Managers/WindowManager.h"
#include "Managers/GLFWManager.h"
#include "Managers/TaskManager.h"
#include "Managers/InputManager.h"
#include "Managers/VRAMManager.h"
#include "Managers/EntityManager.h"

#include "Memory/ThreadLocalPool.h"

// [에셋 및 리소스 시스템 연동 헤더]
#include "Layouts/VertexLayout.h"

#include "Managers/AssetManager.h"
#include "Managers/ResourceManager.h"
#include "Shader/Shader.h"
#include "Shader/ShaderDescriptor.h"
#include "Shader/ShaderImporter.h"
#include "Meshes/StaticMesh.h"
#include "Loaders/StaticMeshLoader.h"
#include "Descriptors/StaticMeshDescriptor.h"

// [타입 베이킹]
#include "InitializerModule.h"


using namespace MGF3D;

int main()
{
    // 0. 부트 스트래핑
    if (!Bootstrapper::Init())
    {
        MGF_LOG_FATAL("Engine Bootstrapping Failed!");
        return -1;
    }

    // 1. 엔진 기본 요소 초기화 (타입 베이킹)
    if (!InitializerModule::Init())
    {
        MGF_LOG_FATAL("Engine Initialization Failed!");
        return -1;
    }

    MGF_LOG_INFO("=== MGF3D Integrated System & Window Test ===");
    /*=================================================//
    // [테스트 1] 입력 매핑 및 콜백 바인딩 (초기화 단계)  //
    //=================================================*/
    // 1. ESC 키를 누르면 창이 닫히도록 이벤트 바인딩 (Edge Trigger)
    MGF_INPUT.MapAction("QuitGame", KeyCode::Escape);
    MGF_INPUT.BindAction("QuitGame", InputEvent::Pressed, []()
    {
        MGF_LOG_INFO("ESC Pressed! Requesting window shutdown...");
        glfwSetWindowShouldClose(MGF_WINDOW.GetNativeWindow(), true);
    });

    // 2. 마우스 좌클릭 시 공격 로그 출력
    MGF_INPUT.MapMouseAction("Fire", MouseCode::Left);
    MGF_INPUT.BindAction("Fire", InputEvent::Pressed, []() { MGF_LOG_INFO("Pew Pew! Left Mouse Button Pressed!"); });

    // 3. W키 매핑 (이건 아래 폴링 루프에서 지속적으로 검사할 예정입니다)
    MGF_INPUT.MapAction("MoveForward", KeyCode::W);

    /*=================================================//
    //   [테스트] 비동기 에셋 및 리소스 로딩 파이프라인   //
    //=================================================*/
    // TODO : 이젠 이 스코프가 없어도 알아서 정리가 되도록 수정해야 함.
    {
        // TODO : 지금 이게 로더를 가져오는 방식이라서 지금 이렇게하면 당연히 로드가 안됨. "로더"를 넣어줘야함.
        auto vsDesc = MakeUnique<ShaderDescriptor>(Shader::s_type.Get(), "BasicVS", "@GameAsset/Shaders/test.vert", ".vert", GL_VERTEX_SHADER);
        auto fsDesc = MakeUnique<ShaderDescriptor>(Shader::s_type.Get(), "BasicFS", "@GameAsset/Shaders/test.frag", ".frag", GL_FRAGMENT_SHADER);
        auto vsAsset = MGF3D::StaticSharedCast<Shader>(MGF_ASSET.LoadAssetAsync(Move(vsDesc)));
        auto fsAsset = MGF3D::StaticSharedCast<Shader>(MGF_ASSET.LoadAssetAsync(Move(fsDesc)));

        // 2. 메쉬 비동기 로드 요청
        auto meshDesc = MakeUnique<StaticMeshDescriptor>(StaticMesh::s_type.Get(), "TriangleMesh");
        StaticGeometryData triGeo;

        // 정점 세팅: Position, Normal, UV, Tangent 순서
        // UV 값을 다르게 주어 프래그먼트 셰이더에서 색상으로 활용합니다.
        triGeo.vertices.PushBack({ vec3(0.0f,  0.5f, 0.0f), vec3(0,0,1), vec2(0.5f, 1.0f), vec3(1,0,0) }); // Top
        triGeo.vertices.PushBack({ vec3(-0.5f, -0.5f, 0.0f), vec3(0,0,1), vec2(0.0f, 0.0f), vec3(1,0,0) }); // Bottom Left
        triGeo.vertices.PushBack({ vec3(0.5f, -0.5f, 0.0f), vec3(0,0,1), vec2(1.0f, 0.0f), vec3(1,0,0) });  // Bottom Right
        triGeo.indices = { 0, 1, 2 };

        meshDesc->geometryList.PushBack(triGeo);
        auto triMesh = MGF3D::StaticSharedCast<StaticMesh>(MGF_RESOURCE.LoadResourceAsync(Move(meshDesc)));

        // 렌더링 상태 변수
        uint32 shaderProgram = 0;
        bool isLinked = false;

        /*=================================================//
        //                 메인 게임 루프                   //
        //=================================================*/
        while (!MGF_WINDOW.ShouldClose())
        {
            MGF3D::ThreadLocalPool::Get()->Reset();

            // 0. 시간 업데이트
            MGF_TIME.Update();

            // 1. OS 이벤트 처리
            MGF_GLFW.Update();

            // 2. 인풋 시스템 상태 동기화
            MGF_INPUT.Update(MGF_WINDOW.GetNativeWindow());
            if (MGF_INPUT.GetButtonDown("MoveForward"))
                MGF_LOG_INFO("W key JUST PRESSED this frame!");

            // 3. 엔티티 시스템 업데이트 (게임 오브젝트들의 상태 업데이트, 추가/제거 처리 등)
            MGF_ENTITY.Update();

            // 4. 메인 스레드 일감 작업
            // 5. 대기 중인 로딩 완료 객체들의 OnCommit을 메인 스레드에서 GPU로 전송
            MGF_TASK.Update();
            MGF_ASSET.Update();
            MGF_RESOURCE.Update();

            // [DEBUG]
            {
                glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // 4. 로딩이 모두 완료되면 단 한 번만 셰이더 프로그램 링크
                if (!isLinked && vsAsset->IsReady() && fsAsset->IsReady() && triMesh->IsReady())
                {
                    shaderProgram = glCreateProgram();
                    glAttachShader(shaderProgram, vsAsset->GetShader());
                    glAttachShader(shaderProgram, fsAsset->GetShader());
                    glLinkProgram(shaderProgram);

                    isLinked = true;
                    MGF_LOG_INFO("All Assets Loaded & Shader Linked! Rendering Started.");
                }

                // 6. 링크가 완료되었다면 삼각형 렌더링
                if (isLinked)
                {
                    glUseProgram(shaderProgram);

                    // StaticMesh 내부의 서브 메쉬를 순회하며 그리기
                    const auto& subMeshes = triMesh->GetSubMeshes();
                    for (const auto& subMesh : subMeshes)
                    {
                        // VertexLayout(VAO)과 IndexBuffer(IBO)가 정상적으로 존재하는지 확인
                        if (subMesh.vertexLayout && subMesh.indexBuffer)
                        {
                            // 1. VAO 바인딩
                            // 이 안에 이미 DSA로 VBO(오프셋 포함)와 IBO 핸들이 세팅되어 있습니다.
                            subMesh.vertexLayout->Bind();

                            // 2. 인덱스 버퍼의 VRAM 서브 할당 오프셋 가져오기
                            // (GLBufferHandle을 상속받았으므로 GetOffset() 사용 가능)
                            uint64 indexOffset = subMesh.indexBuffer->GetOffset();

                            // 3. 그리기 명령 (마지막 인자에 인덱스 오프셋을 캐스팅해서 전달!)
                            glDrawElements
                            (
                                GL_TRIANGLES,
                                subMesh.indexCount,
                                GL_UNSIGNED_INT,
                                reinterpret_cast<const void*>(indexOffset)
                            );

                            // 4. 언바인딩 (선택 사항이지만 안전을 위해 추가 가능)
                            subMesh.vertexLayout->Unbind();
                        }
                    }
                }
            }

            // 3. 렌더링 결과 출력 (SwapBuffers)
            MGF_WINDOW.Update();
        }
    }

    // [체크포인트 C] 엔진 종료 직전 (메모리가 모두 반환되었는지 확인 가능)
    MGF_LOG_WARN(">> [Checkpoint C] Final Memory State before Exit");
    MGF_MEMORY_PROFILE_CAPTURE();
    MGF_MEMORY_LOG_STATS();

    // 최종 정리
    InitializerModule::Shutdown();
    Bootstrapper::Shutdown();

    return 0;
}