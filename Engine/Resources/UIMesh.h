#pragma once
#include "Resources/Mesh.h"

CLASS_PTR(UIMesh)
class UIMesh : public Mesh
{
public:
	virtual ~UIMesh();
	static UIMeshUPtr Create();
	static const ResourceType s_ResourceType = ResourceType::UIMesh;
	virtual ResourceType GetResourceType() const override { return ResourceType::UIMesh; }
	virtual void Draw() const override;

private:
	UIMesh();
	void Init();
};