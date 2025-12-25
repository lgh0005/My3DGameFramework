#pragma once
#include "Core/Scene.h"

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
	PBRScene() = default;
	virtual bool LoadNessesaryResources()	   override;
	virtual bool CreateNessesaryRenderPasses() override;
	virtual bool CreateSceneContext()		   override;

/*==============//
//   for test   //
//==============*/
private:
	void TestSpheresForORMTexture(HDRRenderPass* hdrPass);
	void TestSpheresForPBRChart(HDRRenderPass* hdrPass);
	void TestSpheresForPBRChartDeferred();
};