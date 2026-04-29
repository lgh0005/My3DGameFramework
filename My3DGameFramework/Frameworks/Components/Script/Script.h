#pragma once
#include "Entities/Component.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Script)
	class Script : public Component
	{
		MGF_DISABLE_COPY(Script)
		using Super = Component;

	public:
		Script(ObjectIDHash id, ObjectIDHash ownerID);
		virtual ~Script() override;
		Script(Script&& other) noexcept;
		Script& operator=(Script&& other) noexcept;

	/*==============================//
	//       Script Type            //
	//==============================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	/*======================================//
	//       Script virtual methods         //
	//======================================*/
	public:
		virtual void Awake();
		virtual void Start();
		virtual void Update();
		virtual void FixedUpdate();
		virtual void LateUpdate();

	public:
		virtual void OnEnable();
		virtual void OnDisable();

	public:
		virtual void OnCollisionEnter();
		virtual void OnCollisionStay();
		virtual void OnCollisionExit();
	};
}