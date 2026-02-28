# 🏗️ My3DGameFramework
본 프로젝트는 클라이언트 게임 개발자를 목표로, 게임 엔진의 핵심 로직을 밑바닥부터 직접 구현하며 학습하는 개인 엔진 프레임워크 개발 프로젝트입니다. 특정 상용 엔진에 의존하지 않고 렌더링 파이프라인과 물리 엔진을 통합하며 엔진의 구조적 메커니즘을 이해하는 데 목적을 두고 있습니다.

| Category | Libraries & API | Details |
| :--- | :--- | :--- |
| **Graphics API** | **OpenGL 4.6 Core** | Modern OpenGL Pipeline, GLSL |
| **Window & Input** | **GLFW / GLAD** | OS Context & Extension Loading |
| **Physics** | **Jolt Physics** | High-performance 3D Physics Engine |
| **Audio** | **miniaudio** | Single-header audio playback library |
| **Asset Loading** | **Assimp** | 3D Model Import (fbx, obj, gltf 등) |
| **Math / Tools** | **GLM / ImGui** | OpenGL Mathematics / Debug GUI |


---

# 📺 Development Milestones (Progress Videos)
현재까지의 주요 핵심 개발 진행 상황을 시각적으로 기록합니다.
1. PBR (Physically Based Rendering) & Bloom
<img width="1976" height="789" alt="pbr_chart_and_model" src="https://github.com/user-attachments/assets/1203d3b6-3744-43e7-a40f-3fee366f2a90" />
<img width="1191" height="828" alt="pbr_bloom" src="https://github.com/user-attachments/assets/b9c60bfe-c213-4c27-8602-d1eb993eb8b1" />
모델에 metallic, roughness(또는 glossiness)를 IBL로 인한 재질 표현 및 차트로 PBR의 핵심을 테스트 하였습니다.

2. 여러 가지 Post-Processing
   
https://github.com/user-attachments/assets/4dcd0b9c-9887-4605-9beb-d3a686255ce3

다양한 시각 효과를 위해 더블 버퍼링 기반의 포스트 프로세싱 시스템을 구축하였으며, 다음과 같은 효과들을 구현하였습니다.
구현된 효과 목록 (참고 자료: [3D Game Shaders for Beginners](https://github.com/lettier/3d-game-shaders-for-beginners))
- Color & Tone: ACES / Reinhard / Exposure Tone Mapping
- Lens & Camera: 가우시안 및 Kawase Bloom, 색수차(Chromatic Aberration), 렌즈 플레이어, DOF(Depth of Field), 모션 블러
- Artistic & Effects: 픽셀화, 포스터화, 아웃라이닝(Outlining), CRT 화면 왜곡 효과
- Environment: 포그(Fog), 안티 앨리어싱(FXAA)

3. Jolt-Physics 기반 물리 시뮬레이션
충돌과 Rigidbody 컴포넌트를 구현하는데 사용되었습니다.

https://github.com/user-attachments/assets/6ff0dc71-eaa0-495c-8b5f-374010f9bdc5

4. 핵심 아키텍처 및 파이프라인
엔진의 효율적인 연산을 위해 다음의 렌더링 파이프라인을 설계하였습니다.

> Shadow/Culling → Geometry(G-Buffer) → SSAO → Deferred Lighting → Forward Shading → Skybox → Post-Processing

- 애니메이션 시스템: 상태 머신 기반의 애니메이션 블렌딩 및 본(Bone) 트랜스폼 연산 
- 구현레이캐스팅(Raycasting): 마우스 피킹 및 물리 상호작용을 위한 레이-오브젝트 충돌 시스템
- 메모리 최적화를 위하여 AssetConverter를 통한 3D 모델 파일을 바이너리 파일인 .mymodel, 애니메이션은 .myanim으로 구울 수 있도록 하였고 텍스쳐는 .ktx 포멧으로 변환하는 기능을 추가하였습니다. 추가로 AO, Roughness(또는 Glossiness), Metallic 텍스쳐를 한 데 모아서 ORM 텍스쳐 패킹 시스템을 도입하였습니다.

---

# 🛠️ Future Roadmap & Work in Progress (WIP)
이후에 추가적으로 구현 및 학습 내용들을 나열합니다.
1. recast를 이용한 NavMesh 구현
2. SSGI(Screen Space Global Illumination) 구현
3. 컴퓨트 셰이더를 이용한 Animation Instancing 및 최적화
4. CSM(Cascade Shadow Map), Point Light 그림자 매핑, Light Volume 컬링
5. UI 시스템(World Space, Screen Space), Billboard
6. Area Light
7. Spline 곡선
8. 추가적인 음향 시스템 기능 추가(도플러 효과, 공간 음향, 이퀄라이저)
9. Thread-Management 및 메모리 풀링 커스텀 STL 컨테이너
10. Particle System
11. Lua 스크립팅 연동
12. Archetype 기반 ECS 시스템
12. 기타 기능들 (Cloth, 오브젝트 모션 블러, GPU-Driving 등)
