#pragma once

// C++
#include <cstdarg>
#include <cstdint>
#include <climits>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <optional>
#include <functional>
#include <type_traits>
#include <memory_resource>
#include <atomic>
#include <thread>
#include <mutex>
namespace fs = std::filesystem;

// nlohmann-json
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/json.hpp> 
using json = nlohmann::json;

// OpenGL
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/constants.hpp>

// Jolt
#include <Jolt/Jolt.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Body/BodyID.h>

// Misc
#include "Misc/Defines.h"
#include "Misc/Types.h"
#include "Misc/Utils.h"
#include "Misc/ImguiManager.h"
#define IMGUI     ImGuiManager::Instance()

// Logger
#include "Core/Logger.h"
#define LOGGER	  Logger::Instance()

// Managers
#include "Managers/TimeManager.h"
#include "Managers/WindowManager.h"
#include "Managers/InputManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"
#include "Managers/RenderManager.h"
#include "Managers/AudioManager.h"
#include "Managers/PhysicsManager.h"
#include "Managers/UIManager.h"
#include "Managers/ObjectManager.h"

// TODO : 일부 외부 헤더 때문에 네이밍 컨벤션이
// 충돌하는 모양. 이름을 조금 수정해볼 필요가 있음.
#define TIME	  TimeManager::Instance()
#define WINDOW	  WindowManager::Instance()
#define INPUT_MGR	  InputManager::Instance()
#define RESOURCE  ResourceManager::Instance()
#define SCENE	  SceneManager::Instance()
#define RENDER    RenderManager::Instance()
#define AUDIO     AudioManager::Instance()
#define PHYSICS   PhysicsManager::Instance()
#define UI		  UIManager::Instance()
#define OBJECT	  ObjectManager::Instance()
