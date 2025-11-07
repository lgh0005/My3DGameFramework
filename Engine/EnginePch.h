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

// ImGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Misc
#include "Misc/Defines.h"
#include "Misc/Types.h"
