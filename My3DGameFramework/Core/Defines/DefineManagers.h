#pragma once

/*======================================//
//     Default Core Module Managers     //
//======================================*/
#define MGF_INPUT    ::MGF3D::InputManager::Instance()
#define MGF_TIME     ::MGF3D::TimeManager::Instance()
#define MGF_WINDOW   ::MGF3D::WindowManager::Instance()
#define MGF_TYPE     ::MGF3D::TypeManager::Instance()
#define MGF_NAME   ::MGF3D::NameManager::Instance()
#define MGF_ENTITY ::MGF3D::EntityManager::Instance()