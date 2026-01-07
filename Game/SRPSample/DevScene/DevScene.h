#pragma once
#include "Scene/Scene.h"

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
	DevScene();
	virtual bool LoadSceneResources()	    override;
	virtual bool CreateCustomRenderPasses() override;
	virtual bool SetupSceneEnvironment()	override;
	virtual bool OnPlaceActors()			override;
	virtual bool OnBeginPlay()				override;

/*==============//
//   for test   //
//==============*/
private:
	void PlantTenThousandGrass(InstancedRenderPass* pass);
	void ManyCubesForStressTest();
	void FallRandomCubes();
};