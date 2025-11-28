#pragma once

// C++
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

// Logger
#include <spdlog/spdlog.h>

// OpenGL
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

// Misc
#include "Misc/Defines.h"
#include "Misc/Types.h"
#include "Misc/Utils.h"
#include "Misc/ImguiManager.h"
#define IMGUI	  ImGuiManager::Instance()

// Managers
#include "Managers/TimeManager.h"
#include "Managers/WindowManager.h"
#include "Managers/InputManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/SceneManager.h"

#define TIME	  TimeManager::Instance()
#define WINDOW	  WindowManager::Instance()
#define INPUT	  InputManager::Instance()
#define RESOURCE  ResourceManager::Instance()
#define SCENE	  SceneManager::Instance()