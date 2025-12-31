#pragma once
#include "UIRenderer.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(UIMesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
#pragma endregion

CLASS_PTR(UIImage)
class UIImage : public UIRenderer
{
	using Super = UIRenderer;

public:
	virtual ~UIImage();
	static UIImageUPtr Create(UIMeshPtr mesh, MaterialPtr material);
	static const ComponentType s_ComponentType = ComponentType::UIImage;
	virtual ComponentType GetComponentType() const override { return ComponentType::UIImage; }
	virtual RenderBounds GetWorldBounds() const override { return Super::GetWorldBounds(); }
	virtual void Render(Program* program) override;

private:
	UIImage();
	bool Init(UIMeshPtr mesh, MaterialPtr material);
};