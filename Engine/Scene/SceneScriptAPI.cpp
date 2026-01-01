#include "EnginePch.h"
#include "SceneScriptAPI.h"

Scene* SceneScriptAPI::GetCurrentScene()
{
	return SCENE.GetActiveScene();
}