#pragma once
#include "Core/Scene.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(InstancedRenderPass)
#pragma endregion

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

/*==============//
//   for test   //
//==============*/
private:
	void PlantTenThousandGrass(InstancedRenderPass* pass);
	void ManyCubesForStressTest();
};