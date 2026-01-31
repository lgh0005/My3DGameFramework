#pragma once
#include "Instancing/RenderBatch.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(GraphicsProgram)
CLASS_PTR(InstanceBuffer)
#pragma endregion

CLASS_PTR(RenderQueue)
class RenderQueue
{
public:
	virtual ~RenderQueue() = default;

	virtual bool Init(uint32 maxInstanceCount) = 0;
	virtual void Clear() = 0;
	virtual void Execute(GraphicsProgram* prog) = 0;

protected:
	RenderQueue() = default;
};