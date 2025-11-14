#pragma once
#include "Components/Bases/Component.h"
#include "Components/Transform.h"

// TODO : 
// 1. Destroy
// 2. SetActive
// 3. DontDestroyOnLoad

CLASS_PTR(GameObject)
class GameObject 
{
public:
	static GameObjectUPtr Create();

	// 컴포넌트 메서드들
	void AddComponent(ComponentUPtr component);
	template<typename T>
	T* GetComponent() const;

	// 컴포넌트 getter와 setter들
	Transform& GetTransform()    const	     { return *m_transform; }
	const std::string& GetName() const		 { return m_name; }
	void SetName(const std::string& name)	 { m_name = name; }
	const std::vector<ComponentUPtr>& GetAllComponents() 
								 const		 { return m_components; }

	// TODO : Destroy와 SetActive 메서드 처리


private:
	GameObject() = default;
	bool Init();

	// 트랜스폼 참조용 포인터
	Transform*  m_transform					 { nullptr };
	std::string m_name						 { "GameObject" };
	std::vector<ComponentUPtr> m_components;
};

/*=================================//
//   game object template inlines  //
//=================================*/
#pragma region GAME_OBJECTS_INLINES
template<typename T>
inline T* GameObject::GetComponent() const
{
	for (const auto& comp : m_components)
	{
		if (comp->GetType() == T::s_ComponentType)
			return static_cast<T*>(comp.get());
	}
	return nullptr;
}
#pragma endregion