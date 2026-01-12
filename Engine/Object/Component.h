#pragma once
#include "Misc/Enums.h"
#include "Object.h"
#include "Managers/ObjectManager.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(GameObject)
CLASS_PTR(Transform)
#pragma endregion

CLASS_PTR(Component)
class Component : public Object
{
	friend class GameObject;

public:
	static constexpr usize INVALID_REGISTRY_IDX = static_cast<RegistryIndex>(-1);

public:
	virtual ~Component();
	virtual ComponentType GetComponentType() const = 0;
	virtual bool MatchesType(ComponentType type) const;

	void SetRegistryIndex(usize idx) { m_registryIndex = idx; }
	usize GetRegistryIndex() const { return m_registryIndex; }

	ComponentState GetState() const { return m_state; }
	GameObject* GetOwner() const;
	Transform& GetTransform();				// reference transform getter
	const Transform& GetTransform() const;  // read-only transform getter

/*============================================//
//   component life-cycle methods by engine   //
//============================================*/
public:
	virtual void Awake()		override final;
	virtual void Start()		override final;
	virtual void FixedUpdate()	override final;
	virtual void Update()		override final;
	virtual void LateUpdate()	override final;
	virtual void OnDestroy()	override final;

/*==========================================//
//   component life-cycle methods by user   //
//==========================================*/
protected:
	virtual void OnAwake();
	virtual void OnStart();
	virtual void OnFixedUpdate();
	virtual void OnUpdate();
	virtual void OnLateUpdate();
	virtual void OnDestroyed();
	virtual void OnEnable();
	virtual void OnDisable();

/*===================================//
//   component state checking API    //
//===================================*/
public:
	void SetComponentState(ComponentState state) { m_state = state; }
	bool IsAwake() const { return m_state >= ComponentState::Awake; }
	bool IsStarted() const { return m_state >= ComponentState::Started; }
	bool IsDead() const { return m_state == ComponentState::Dead; }

/*===========================================================//
//   owner and self(this component) state checking methods   //
//===========================================================*/
public:
	void SetEnable(bool enable);	// 컴포넌트 자체적인 활성화 스위치
	bool IsEnabled() const;			// I) 컴포넌트 자체적으로 enable한가?
	bool IsOwnerActive() const;		// II) 이 컴포넌트의 소유자의 상태가 active한가?
	bool IsActive() const;			// III) 결론적으로 I과 II를 토대로 컴포넌트를 실행해야 하는가?

protected:
	Component();
	void SetOwner(GameObject* gameObject) { m_owner = gameObject; }
	void EnsureInitialized();

	GameObject*    m_owner				  { nullptr };
	bool		   m_enabled		      { true };
	ComponentState m_state				  { ComponentState::Uninitialized };
	RegistryIndex  m_registryIndex		  { INVALID_REGISTRY_IDX };
};