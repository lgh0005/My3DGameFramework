#pragma once

/*=============================//
//    C++ standard libraries   //
//=============================*/
#include <cmath>
#include <type_traits>
#include <utility>
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
#include "Common/CommonUtils.h"

/*============================//
//    Engine common aliases   //
//============================*/
#include "Aliases/ManagerAccess.h"
#include "Aliases/Array.h"
#include "Aliases/Nullable.h"
#include "Aliases/Delegates.h"
#include "Aliases/Threading.h"

/*============================//
//    Engine smart pointers   //
//============================*/
#include "Pointer/SmartPtrAliases.h"
#include "Pointer/SmartPtrCore.h"
#include "Pointer/SmartPtrFactory.h"

/*==============================//
//    Engine common debuggers   //
//==============================*/
#include "Debug/Logger.h"
#include "Debug/Asserter.h"
#include "Debug/MemoryProfiler.h"
#include "Debug/Clock.h"
#include "Debug/ProfileScope.h"
#include "Debug/RealTimer.h"

/*==========================//
//    Engine common utils   //
//==========================*/
#include "Math/Math.h"
#include "Math/Random.h"

/*=========================//
//    Engine common STLs   //
//=========================*/
#include "Containers/Slab/SMap.h"
#include "Containers/Slab/SSet.h"
#include "Containers/Slab/SString.h"
#include "Containers/Slab/SVector.h"
#include "Containers/Slab/SDeque.h"
#include "Containers/Slab/SBitset.h"

#include "Containers/Linear/LMap.h"
#include "Containers/Linear/LSet.h"
#include "Containers/Linear/LString.h"
#include "Containers/Linear/LVector.h"
#include "Containers/Linear/LDeque.h"
#include "Containers/Linear/LBitset.h"

/*================================//
//    Engine resource identities  //
//================================*/
#include "Identity/MGFName.h"
#include "Identity/MGFPath.h"

/*=============================//
//    Engine container utils   //
//=============================*/
#include "Utils/StringUtils.h"
