#pragma once

/*==========================//
//    Physics JPH_ASSERTS   //
//==========================*/
#ifdef _DEBUG
	#define JPH_ENABLE_ASSERTS
#endif	

/*==============//
//    CorePch   //
//==============*/
#include "CorePch.h"

/*=============================//
//    External third-parties   //
//=============================*/
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Memory.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/Reference.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>

/*===============================//
//    Physics common constants   //
//===============================*/
#include "Common/JoltConstants.h"

/*=============================//
//    Physics common defines   //
//=============================*/
#include "Defines/DefineManager.h"

/*=============================//
//    Physics common headers   //
//=============================*/
#include "Common/Types.h"
#include "Common/JPHMath.h"

/*===========================//
//    Physics common utils   //
//===========================*/
#include "Utils/JoltMathBridge.h"