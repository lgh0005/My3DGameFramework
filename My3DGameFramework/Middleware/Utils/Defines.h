#pragma once

/*======================================================//
//     Default Core & Platform Manager Alias Macros     //
//======================================================*/
#define MGF_AI     MGF3D::AIManager::Instance()
#define MGF_AUDIO  MGF3D::AudioManager::Instance()
#define MGF_INSTANCE MGF3D::InstanceManager::Instance()
#define MGF_RENDER MGF3D::RenderManager::Instance()
#define MGF_PHYSICS MGF3D::PhysicsManager::Instance()
#define MGF_COROUTINE MGF3D::CoroutineManager::Instance()
#define MGF_LUA	MGF3D::LuaManager::Instance()
#define MGF_UI	MGF3D::UIManager::Instance()

#define MGF_GLFW  MGF3D::GLFWManager::Instance()
#define MGF_WINDOW MGF3D::WindowManager::Instance()
#define MGF_INPUT MGF3D::InputManager::Instance()
#define MGF_TIME  MGF3D::TimeManager::Instance()

/*=========================================//
//     Manager initialize phrase macro     //
//=========================================*/
#define MGF_INIT_SYS(InitExpr, SystemName)                                        \
    do                                                                            \
    {                                                                             \
        if (!(InitExpr))                                                          \
        {                                                                         \
            MGF_LOG_FATAL("Bootstrapper: Failed to initialize {0}!", SystemName); \
            return false;                                                         \
        }                                                                         \
        MGF_LOG_TRACE("Bootstrapper: {0} initialized successfully.", SystemName); \
    } while(0)