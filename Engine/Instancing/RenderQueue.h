#pragma once
#include "Instancing/RenderBatch.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(InstanceBuffer)
#pragma endregion

CLASS_PTR(RenderQueue)
class RenderQueue
{
public:
	virtual ~RenderQueue();

	virtual bool Init(uint32 maxInstanceCount) = 0;
	virtual void Clear() = 0;
	virtual void Execute(Program* prog) = 0;

protected:
	RenderQueue();
};