#pragma once

/*==============//
//    CorePch   //
//==============*/
#include "CorePch.h"

/*=============================//
//    External third-parties   //
//=============================*/
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

/*===================================//
//    Physics common configuration   //
//===================================*/
#include "Core/JoltConfig.h"

/*=============================//
//    Physics common aliases   //
//=============================*/
#include "Aliases/ManagerAccess.h"

/*=============================//
//    Physics common headers   //
//=============================*/
#include "Utils/Types.h"
#include "Utils/JPHMath.h"