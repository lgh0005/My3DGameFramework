find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(imguizmo CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_library(Engine STATIC

   "EnginePch.h" "EnginePch.cpp"
   "Game.h"      "Game.cpp"
       
   "Misc/Defines.h"
   "Misc/Types.h"
   "Misc/ImguiManager.h"     "Misc/ImguiManager.cpp"

   "Graphics/Shader.h"       "Graphics/Shader.cpp"
   "Graphics/Program.h"      "Graphics/Program.cpp"
   "Graphics/Context.h"      "Graphics/Context.cpp"
   "Graphics/Buffer.h"       "Graphics/Buffer.cpp" 
   "Graphics/VertexLayout.h" "Graphics/VertexLayout.cpp"
   "Graphics/Image.h"        "Graphics/Image.cpp"
   "Graphics/Texture.h"      "Graphics/Texture.cpp" 

   "Components/Camera.h"     "Components/Camera.cpp"
   "Components/Transform.h"  "Components/Transform.cpp"

   "Managers/TimeManager.h"  "Managers/TimeManager.cpp"

   "Managers/WindowManager.h" "Managers/WindowManager.cpp"
   "Managers/InputManager.h" "Managers/InputManager.cpp"
 "Misc/ImguiManager.cpp"

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
    imguizmo::imguizmo
    spdlog::spdlog
    assimp::assimp
)

# 5. [컴파일 정의] Engine 타겟에 C++ 매크로를 정의합니다.
target_compile_definitions(Engine PUBLIC
    WINDOW_WIDTH=1200
    WINDOW_HEIGHT=800
    WINDOW_NAME="My3DGameFramework"
)