#include "EnginePch.h"
#include "ScriptRegistry.h"
#include "Components/Script.h"

std::atomic<ScriptID> ScriptRegistry::s_nextID	{ 0 };