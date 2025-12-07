#pragma once
#include "Core/Scene.h"

CLASS_PTR(PBRScene)
class PBRScene : public Scene
{
public:
	virtual ~PBRScene();
	static PBRSceneUPtr Create();

protected:
	PBRScene() = default;
	virtual bool LoadNessesaryResources()	   override;
	virtual bool CreateNessesaryRenderPasses() override;
	virtual bool CreateSceneContext()		   override;
};