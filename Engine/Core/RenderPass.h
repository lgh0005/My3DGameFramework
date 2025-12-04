#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(RenderContext)
#pragma endregion

/*===========================================//
//   custom extension render pass interface  //
//===========================================*/
CLASS_PTR(GeneralRenderPass)
class GeneralRenderPass
{
public:
	virtual ~GeneralRenderPass();
	virtual void Render(Scene* scene, Camera* camera) = 0;

protected:
	GeneralRenderPass();
};

/*====================================================//
//   RenderContext based core render pass interface   //
//====================================================*/
CLASS_PTR(ContextRenderPass)
class ContextRenderPass
{
public:
	virtual ~ContextRenderPass();
	virtual void Render(RenderContext* context) = 0;

protected:
	ContextRenderPass();
};