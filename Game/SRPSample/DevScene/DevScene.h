#pragma once
#include "Core/Scene.h"

CLASS_PTR(DevScene)
class DevScene : public Scene
{
public:
	virtual ~DevScene();
	static DevSceneUPtr Create();

protected:
	DevScene() = default;
	virtual bool LoadNessesaryResources()	   override;
	virtual bool CreateNessesaryRenderPasses() override;
	virtual bool CreateSceneContext()		   override;
};