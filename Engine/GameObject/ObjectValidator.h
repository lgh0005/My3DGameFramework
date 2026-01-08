#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Component)
#pragma endregion

CLASS_PTR(ObjectValidator)
class ObjectValidator
{
	DECLARE_STATIC_CLASS(ObjectValidator)

/*==============================//
//   validation of GameObject   //
//==============================*/
public:
	static bool IsValidGameObject(const GameObject* go);

/*=============================//
//   validation of Component   //
//=============================*/
public:
	static bool IsValidComponent(const Component* comp);
	static bool IsActiveInHierarchy(const Component* comp);
};