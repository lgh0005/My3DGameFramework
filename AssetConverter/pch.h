#pragma once

// C++
#include <iostream>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <unordered_set>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <filesystem>
#include <fstream>
#include <memory>
namespace fs = std::filesystem;

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
#include <glm/gtx/matrix_decompose.hpp>

// stb & ktx
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize2.h>
#include <ktx.h>

// Utils
#include "Utils/Types.h"
#include "Utils/Defines.h"
#include "Utils/Utils.h"
#include "Utils/Logger.h"

// AssetUtils
#include "AssetTypes/AssetFormat.h"
#include "AssetTypes/AssetUtils.h"

// Converters
#include "Converters/ModelConverter.h"
#include "Converters/AnimationConverter.h"
#include "Converters/ORMTexturePacker.h"
#include "Converters/KTXTextureConverter.h"

#define CONV_MODEL ModelConverter::Instance()
#define CONV_ANIM  AnimationConverter::Instance()
#define CONV_ORM   ORMTexturePacker::Instance()
#define CONV_KTX   KTXTextureConverter::Instance()