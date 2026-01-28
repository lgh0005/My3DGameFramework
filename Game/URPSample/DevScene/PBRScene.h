#pragma once
#include "Scene/Scene.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(HDRRenderPass)
#pragma endregion

CLASS_PTR(PBRScene)
class PBRScene : public Scene
{
public:
	virtual ~PBRScene();
	static PBRSceneUPtr Create();

protected:
	PBRScene();
	virtual bool LoadSceneResources()	    override;
	virtual bool CreateCustomRenderPasses() override;
	virtual bool OnPlaceActors()			override;
	virtual bool OnBeginPlay()				override;

/*==============//
//   for test   //
//==============*/
private:
	void TestSpheresForORMTexture(HDRRenderPass* hdrPass);
	void TestSpheresForPBRChart(HDRRenderPass* hdrPass);
	void TestSpheresForPBRChartDeferred();
};