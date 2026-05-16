#pragma once
#include "Components/Script/Script.h"

namespace MGF3D
{
	class PlayerController : public Script
	{
		MGF_DISABLE_COPY(PlayerController)
		using Super = Script;

	public:
		PlayerController(ObjectIDHash id, ObjectIDHash ownerID);
		virtual ~PlayerController() override;
		PlayerController(PlayerController&& other) noexcept;
		PlayerController& operator=(PlayerController&& other) noexcept;

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
		virtual void Awake() override;
		virtual void Update() override;

	private:
		void ProcessKeyboard(float dt);
		void ChangeAnimation();

		float m_moveSpeed	{ 2.0f };
		bool m_isMoving		{ false };
	};
}