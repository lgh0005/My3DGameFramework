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
#include "Common/Types.h"
#include "Common/Defines.h"
#include "Common/Constants.h"
#include "Common/Math.h"

#include "Utils/CommonUtils.h"

#include "Common/Pointers.h"
#include "Common/Nullable.h"
#include "Common/Delegates.h"
#include "Common/Sync.h"
#include "Common/Random.h"
#include "Common/Clock.h"

///*=========================//
////    Engine common STLs   //
////=========================*/
//#include "Containers/Slab/SMap.h"
//#include "Containers/Slab/SSet.h"
//#include "Containers/Slab/SString.h"
//#include "Containers/Slab/SVector.h"
//
//#include "Containers/Linear/LMap.h"
//#include "Containers/Linear/LSet.h"
//#include "Containers/Linear/LString.h"
//#include "Containers/Linear/LVector.h"
