#pragma once
#include "Entities/Mechanic.h"
#include "Mechanics/Animation/Pose.h"

namespace MGF3D
{
	MGF_CLASS_PTR(Animation)
	MGF_CLASS_PTR(AnimState)

	MGF_CLASS_PTR(AnimController)
	class AnimController : public Mechanic
	{
		MGF_DISABLE_COPY(AnimController)
		using Super = Mechanic;

	public:
		AnimController(Component* owner);
		virtual ~AnimController() override;
		AnimController(AnimController&& other) noexcept;
		AnimController& operator=(AnimController&& other) noexcept;

	/*================================//
	//   MGF3D Mechanic Custom Type   //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const override;

	/*================================//
	//       Animation Controls       //
	//================================*/
	public:
		void AddState(const String& name, const AnimationPtr& clip, bool isLoop = true, float speed = 1.0f);
		void SetTransitionDuration(StringHash fromHash, StringHash toHash, float duration);

		void SetStartState(StringHash stateHash);
		void Play(StringHash stateHash);
		void CrossFade(StringHash nextStateHash, float duration = -1.0f);

		void Update(float deltaTime);

		AnimState* GetCurrentState() const { return m_currentState.get(); }
		Pose GetPose(StringHash nodeHash, const Pose& defaultPose) const;
		mat4 GetLocalMatrix(StringHash nodeHash, const mat4& defaultMatrix) const;

	private:
		void TransitTo(const AnimStatePtr& nextState, float duration);

		HashMap<StringHash, AnimStatePtr> m_states;

		// 현재/다음 상태
		AnimStatePtr m_currentState;
		AnimStatePtr m_nextState;

		// 시간 관리
		float m_currentTime{ 0.0f };
		float m_nextTime{ 0.0f };

		// 블렌딩 제어
		bool  m_isBlending{ false };
		float m_blendTimer{ 0.0f };
		float m_blendDuration{ 0.0f };
		float m_finalBlendFactor{ 0.0f };
	};
}