#pragma once
#include "ISingleton.h"

class Camera;
class IRenderable;

class RenderManager : public ISingleton<RenderManager>
{
	DECLARE_SINGLE(RenderManager);

public:
	virtual ~RenderManager() override;

public:
	virtual void Init() override;
	virtual void Update() override;
};