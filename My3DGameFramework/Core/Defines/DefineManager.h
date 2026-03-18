#pragma once

/*==============================================//
//     Default Core library manager aliases     //
//==============================================*/
#define MGF_MEMORY	 MGF3D::MemoryManager::Instance()
#define MGF_JOB		 MGF3D::JobManager::Instance()
#define MGF_TASK	 MGF3D::TaskManager::Instance()
#define MGF_PATH	 MGF3D::PathManager::Instance()
#define MGF_NAME	 MGF3D::NameManager::Instance()
#define MGF_ENTITY	 MGF3D::EntityManager::Instance()
#define MGF_RESOURCE MGF3D::ResourceManager::Instance()
#define MGF_TYPE	 MGF3D::TypeManager::Instance()