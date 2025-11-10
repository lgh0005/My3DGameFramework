#pragma once

// C++
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>

// Logger
#include <spdlog/spdlog.h>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

// Engine
#include "Misc/Utils.h"
#include "Misc/Types.h"
#include "Misc/Defines.h"

#include "Graphics/Vertex.h"
#include "Graphics/Bone.h"

// Utils
#include "Utils/ConverterUtils.h"