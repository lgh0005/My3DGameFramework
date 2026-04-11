#pragma once

/*=============================//
//    C++ standard libraries   //
//=============================*/
#include <cmath>
#include <utility>
#include <type_traits>

/*=============================//
//    External third-parties   //
//=============================*/
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>
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

/*==========================//
//    Engine common types   //
//==========================*/
#include "Types/Types.h"
#include "Types/Math.h"
#include "Types/Containers.h"
#include "Types/Strings.h"
#include "Types/SmartPtrs.h"
#include "Types/Delegates.h"
#include "Types/FileStream.h"
#include "Types/Threading.h"

/*==============================//
//    Engine common constants   //
//==============================*/
#include "Constants/ConstantMath.h"

/*============================//
//    Engine common defines   //
//============================*/
#include "Defines/DefineGLExtensions.h"
#include "Defines/DefineClassPointers.h"
#include "Defines/DefineStructPointers.h"
#include "Defines/DefineDisables.h"
#include "Defines/DefineSingleton.h"
#include "Defines/DefineUtilities.h"
#include "Defines/DefineHash.h"
#include "Defines/DefineManagers.h"

/*==============================//
//    Engine common debuggers   //
//==============================*/
#include "Debug/Logger.h"
#include "Defines/DefineLoggings.h"
#include "Debug/Asserter.h"
#include "Defines/DefineAsserter.h"
#include "Debug/Stopwatch.h"
#include "Defines/DefineTimeStamp.h"

/*===========================//
//    Engine common hashes   //
//===========================*/
#include "Hashes/StringHash.h"
#include "Hashes/TypeHash.h"

/*===============================//
//    Engine common identities   //
//===============================*/
#include "Identities/MGFPath.h"
#include "Identities/MGFType.h"
#include "Identities/MGFTypeCaster.h"

/*==============================//
//    Engine common utilities   //
//==============================*/
#include "CoreUtils/CommonUtils.h"
#include "CoreUtils/MathUtils.h"