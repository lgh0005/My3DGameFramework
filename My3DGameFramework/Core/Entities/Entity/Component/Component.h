#pragma once
#include "Entities/Entity/Object.h"
#include "Entities/Entity/Component/IComponentDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(GameObject)
	MGF_CLASS_PTR(ComponentStateMachine)

	MGF_CLASS_PTR(Component)
	class Component : public Object
	{
		template<typename T> friend class Storage;

	public:
		virtual ~Component();
		Ptr<ComponentStateMachine> GetComponentStateMachine() const;
		
		template<typename T, typename... Args>
		static Ptr<T> Create(Ptr<GameObject> owner, Args&&... args);

	/*==================================//
	//         Type System              //
	//==================================*/
	public:
		static UniquePtr<MGFType> s_type;
		static void InitializeType();
		virtual const MGFType* GetType() const;

	/*===================================//
	//   component getters and setters   //
	//===================================*/
	public:
		void SetOwner(Ptr<GameObject> gameObject) { m_owner = gameObject; }
		Ptr<GameObject> GetOwner() const { return m_owner; }

		void SetStorageIndex(usize index) { m_storageIndex = index; }
		usize GetStorageIndex() const { return m_storageIndex; }

		bool IsEnable() const;
		void SetEnable(bool enable);

	/*================================//
	//   common component utilities   //
	//================================*/
	public:
		// 1. Transform 접근자
		// ... 

	/*============================================//
	//   component life-cycle methods by engine   //
	//============================================*/
	public:
		virtual void Awake();
		virtual void Start();
		virtual void FixedUpdate();
		virtual void Update();
		virtual void LateUpdate();
		virtual void OnDestroy();

	protected:
		Component(const IComponentDescriptor& desc);
		Ptr<GameObject>	   m_owner{ nullptr };
		usize			   m_storageIndex{ 0 };
		const Ptr<MGFType> m_type;
		MGFName			   m_name;
	};
}

#include "Entities/Entity/Component/Component.inl"