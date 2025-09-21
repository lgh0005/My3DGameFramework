#pragma once

// C++20 STL
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <queue>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <concepts>
#include <type_traits>
#include <fstream>
#include <sstream>
#include <format>
using namespace std;

// SDL
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#pragma comment(lib, "SDL/SDL2.lib")
#pragma comment(lib, "SDL/SDL2main.lib")
#pragma comment(lib, "SDL/SDL2_image.lib")
#pragma comment(lib, "SDL/SDL2_ttf.lib")

// FMOD
#include <Fmod/fmod_studio.hpp>
#include <Fmod/fmod.hpp>
#include <Fmod/fmod_common.h>
#include <Fmod/fmod_codec.h>
#include <Fmod/fmod_errors.h>
using namespace FMOD;

#ifdef _DEBUG
#pragma comment(lib, "Fmod/Debug/fmodL_vc.lib")
#pragma comment(lib, "Fmod/Debug/fmodstudioL_vc.lib")
#else
#pragma comment(lib, "Fmod/Release/fmod_vc.lib")
#pragma comment(lib, "Fmod/Release/fmodstudio_vc.lib")
#endif

// OpenGL
#include <glew/GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma comment(lib, "glew/glew32.lib")
#pragma comment(lib, "opengl32.lib")

// Misc
#include "Types.h"
#include "Defines.h"
#include "Values.h"
#include "Enums.h"

// Managers
#include "TimeManager.h"
#include "InputManager.h"
#include "RenderManager.h"

#define TIME TimeManager::GetInstance()
#define INPUT InputManager::GetInstance()
#define RENDER RenderManager::GetInstance()