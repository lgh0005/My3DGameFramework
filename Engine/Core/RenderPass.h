#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Scene)
CLASS_PTR(Camera)
CLASS_PTR(RenderContext)
#pragma endregion

/*====================================================//
//  Base class for all render passes (Common Data)    //
//====================================================*/
class RenderPass
{
public:
	virtual ~RenderPass();

protected:
	RenderPass();
	const std::vector<glm::mat4>& GetIdentityBones() const;

private:
	static std::vector<glm::mat4> s_identityBones;
};

/*===========================================//
//   custom extension render pass interface  //
//===========================================*/
CLASS_PTR(GeneralRenderPass)
class GeneralRenderPass : public RenderPass
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
class ContextRenderPass : public RenderPass
{
public:
	virtual ~ContextRenderPass();
	virtual void Render(RenderContext* context) = 0;

protected:
	ContextRenderPass();
};