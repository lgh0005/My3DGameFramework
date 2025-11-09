#pragma once

// C++
#include <iostream>
#include <string>
#include <vector>

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
#define IMGUI ImGuiManager::Instance()

#include "Graphics/VertexLayout.h" // TEMP : 어디에서 이걸 포함하지 않은 건지 추적 필요

// Managers
#include "Managers/TimeManager.h"
#include "Managers/WindowManager.h"
#include "Managers/InputManager.h"

#define TIME TimeManager::Instance()
#define WINDOW WindowManager::Instance()
#define INPUT InputManager::Instance()