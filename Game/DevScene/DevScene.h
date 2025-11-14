#pragma once
#include "Core/Scene.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
#pragma endregion

// TODO : 큐브 하나를 렌더링
// 1. 프레임 버퍼 기반 렌더링

CLASS_PTR(DevScene)
class DevScene : public Scene
{
public:
	static DevSceneUPtr Create();
	virtual ~DevScene() = default;

protected:
	DevScene() = default;
	virtual bool Init() override;
};