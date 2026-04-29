#pragma once
#include "Components/Script/Script.h"

namespace MGF3D
{
	class CameraController : public Script
	{
		MGF_DISABLE_COPY(CameraController)
		using Super = Script;

	public:
		CameraController(ObjectIDHash id, ObjectIDHash ownerID);
		virtual ~CameraController() override;
		CameraController(CameraController&& other) noexcept;
		CameraController& operator=(CameraController&& other) noexcept;

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
		void ProcessMouse(float dt);

	private:
		float m_moveSpeed = 5.0f;
		float m_lookSensitivity = 0.15f;

		float m_yaw = 0.0f;
		float m_pitch = 0.0f;
	};
}