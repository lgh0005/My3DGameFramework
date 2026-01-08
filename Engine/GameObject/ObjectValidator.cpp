#include "EnginePch.h"
#include "ObjectValidator.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"

/*==============================//
//   validation of GameObject   //
//==============================*/
bool ObjectValidator::IsValidGameObject(const GameObject* go)
{
	if (go == nullptr) return false;
	if (go->IsDead()) return false;
	return true;
}

/*=============================//
//   validation of Component   //
//=============================*/
bool ObjectValidator::IsValidComponent(const Component* comp)
{
	if (comp == nullptr) return false;
	if (comp->GetState() == ComponentState::Dead) return false;
	if (!IsValidGameObject(comp->GetOwner())) return false;
	return true;
}

bool ObjectValidator::IsActiveInHierarchy(const Component* comp)
{
	if (!IsValidComponent(comp)) return false;
	if (!comp->IsEnabled()) return false;
	if (comp->GetState() == ComponentState::Uninitialized) return false;

	GameObject* owner = comp->GetOwner();
	if (owner == nullptr) return false;
	if (owner->IsDead()) return false;

	return owner->IsActiveInHierarchy();
}