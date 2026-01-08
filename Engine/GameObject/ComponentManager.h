#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Component)
CLASS_PTR(StaticMeshRenderer)
CLASS_PTR(SkinnedMeshRenderer)
CLASS_PTR(InstancedMeshRenderer)
CLASS_PTR(MeshOutline)
CLASS_PTR(UICanvas)
CLASS_PTR(Light)
CLASS_PTR(Camera)
CLASS_PTR(Animator)
CLASS_PTR(Script)
CLASS_PTR(AudioSource)
CLASS_PTR(AudioListener)
CLASS_PTR(Collider)
CLASS_PTR(Rigidbody)
#pragma endregion

CLASS_PTR(ComponentManager)
class ComponentManager
{
public:
	// TODO : GameObject는 자신이 소유하는 Component
	// 의 생명 주기를 관리한다.

private:


};