#pragma once

/*=========================================//
//     Default Runtime Module Managers     //
//=========================================*/
#define MGF_LEVEL  ::MGF3D::LevelManager::Instance()
#define MGF_SCENE  ::MGF3D::SceneManager::Instance()
#define MGF_EDITOR ::MGF3D::GUIManager::Instance()
#define MGF_CONSOLE ::MGF3D::ConsoleManager::Instance()