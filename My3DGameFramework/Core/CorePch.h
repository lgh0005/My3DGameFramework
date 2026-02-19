#pragma once

/*=============================//
//    C++ standard libraries   //
//=============================*/
#include <cstdint>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <unordered_map>
#include <optional>
#include <memory>
#include <memory_resource>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <thread>
#include <mutex>
#include <random>
#include <filesystem>
#include <fstream>
#include <sstream>

/*=============================//
//    External third-parties   //
//=============================*/
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>

/*============================//
//    Engine common headers   //
//============================*/
#include "Common/Defines.h"   
#include "Common/Aliases.h"   
#include "Common/Types.h"     
#include "Common/Constants.h"