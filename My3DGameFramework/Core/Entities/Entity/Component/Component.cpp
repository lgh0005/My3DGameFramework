#include "CorePch.h"
#include "Component.h"
#include "ComponentStateMachine.h"
#include "Entities/Entity/GameObject/GameObject.h"

namespace MGF3D
{
	Component::Component(const IComponentDescriptor& desc)
		: m_type(desc.type), m_name(desc.name) 
	{
		m_stateMachine = MakeUnique<ComponentStateMachine>(this);
	}
	Component::~Component() = default;

	/*==================================//
    //         Type System              //
    //==================================*/
	UniquePtr<MGFType> Component::s_type = nullptr;

	void Component::InitializeType()
	{
		s_type = MakeUnique<MGFType>();
		s_type->name = MGFName("Component");
		s_type->id = StringHash("Component");

		// 최상위 루트이므로 부모 없음
		s_type->parent = nullptr;
		s_type->depth = 0;
		s_type->chain[0] = s_type->id;
	}

	const MGFType* Component::GetType() const
	{
		return s_type.Get();
	}

	/*===================================//
	//   component getters and setters   //
	//===================================*/
	bool Component::IsEnable() const
	{
		return m_stateMachine->IsActive();
	}

	void Component::SetEnable(bool enable)
	{
		GetComponentStateMachine()->SetEnabled(enable);
	}

	Ptr<ComponentStateMachine> Component::GetComponentStateMachine() const
	{
		return static_cast<Ptr<ComponentStateMachine>>(m_stateMachine.Get());
	}

	/*============================================//
	//   component life-cycle methods by engine   //
	//============================================*/
	void Component::Awake()			{ }
	void Component::Start()			{ }
	void Component::FixedUpdate()	{ }
	void Component::Update()		{ }
	void Component::LateUpdate()	{ }
	void Component::OnDestroy()		{ }
}