#pragma once

/*==============================================//
//     Default Audio library manager aliases    //
//==============================================*/
#define MGF_PHYSICS			MGF3D::PhysicsManager::Instance()
#define MGF_PHYSICS_QUERY	MGF3D::PhysicsQueryManager::Instance()
#define MGF_COLLISION		MGF3D::CollisionManager::Instance()
#define JPH_MEMORY			MGF3D::JoltMemoryManager::Instance()
#define JPH_JOB				MGF3D::JoltJobManager::Instance()