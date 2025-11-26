#pragma once
#include "Component.h"

CLASS_PTR(Script)
class Script : public Component
{
public:
	static const ComponentType s_ComponentType = ComponentType::Script;
	virtual ComponentType GetType() const override { return ComponentType::Script; }

	// TODO : Awake, FixedUpdate, LateUpdate 등의 분기 필요할 수 있음
	virtual void Start()   {}
	virtual void Update()  {}

	// TODO :
	// 0. OnEnable, OnDisable, OnDestroy
	// 1. 충돌 시 발생할 이벤트 메서드 : OnTrigger, OnCollision
};