#pragma once

// C++
#include <iostream>
#include <cfloat>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <memory>

// assimp
// TODO : Model, Animation 로드 컨버터 클래스에만 포함되도록 빼둘
// 필요가 있음
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

// Utils
#include "Utils/Utils.h"
#include "Utils/Types.h"
#include "Utils/Defines.h"
#include "Utils/Logger.h"

// Converters
#include "Converters/ModelConverter.h"

#define CONV_MODEL ModelConverter::Instance()