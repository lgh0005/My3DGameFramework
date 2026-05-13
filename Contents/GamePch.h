#pragma once

/*============//
//  Core Pch  //
//============*/
#include "ApplicationPch.h"

/*==========================//
//  utilities for contents  //
//==========================*/
#include "GraphicsUtils/GeometryUtils.h"

/*=========================//
//  managers for contents  //
//=========================*/
#include "Managers/TypeManager.h"
#include "Managers/AssetManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/TimeManager.h"

/*=========================//
//  entities for contents  //
//=========================*/
#include "Entities/GameObject.h"

/*=======================//
//  assets for contents  //
//=======================*/
#include "Assets/Image.h"
#include "Assets/Model.h"
#include "Assets/Animation.h"

/*==========================//
//  resources for contents  //
//==========================*/
#include "Resources/Material.h"
#include "Resources/EnvironmentMap.h"

#include "Meshes/Mesh.h"
#include "Meshes/StaticMesh.h"
#include "Meshes/SkinnedMesh.h"

/*==========================//
//  mechanics for contents  //
//==========================*/
#include "Mechanics/Animation/AnimController.h"
#include "Mechanics/Lights/SkyCube.h"

/*===========================//
//  components for contents  //
//===========================*/
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/MeshRenderers/MeshRenderer.h"
#include "Components/Animation/Animator.h"
#include "Components/Lights/DirectionalLight.h"
#include "Components/Lights/PointLight.h"
#include "Components/Lights/SpotLight.h"
#include "Components/Lights/SkyLight.h"