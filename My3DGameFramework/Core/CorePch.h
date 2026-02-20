#pragma once

/*=============================//
//    C++ standard libraries   //
//=============================*/
#include <cmath>
#include <cstdarg>
#include <algorithm>
#include <type_traits>
#include <memory>

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
#include "Common/Types.h"     
#include "Common/Constants.h"
#include "Common/Math.h"

#include "Common/Pointers.h"
#include "Common/Nullable.h"
#include "Common/Delegates.h"
#include "Common/Sync.h"
#include "Common/Random.h"
#include "Common/Time.h"

