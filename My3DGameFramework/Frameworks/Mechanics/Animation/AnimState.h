#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(Animation)

	MGF_CLASS_PTR(AnimState)
	class AnimState
	{
		MGF_DISABLE_COPY(AnimState)

	public:
		// 문자열을 받아 내부적으로 해시와 원본을 모두 세팅합니다.
		AnimState(const String& name, const AnimationPtr& anim);
		~AnimState();

		AnimState(AnimState&& other) noexcept;
		AnimState& operator=(AnimState&& other) noexcept;

	public:
		void AddTransition(StringHash targetStateHash, float duration);
		float GetTransitionDuration(StringHash targetStateHash) const;

		void SetLoop(bool loop) { m_isLoop = loop; }
		void SetSpeed(float speed) { m_speed = speed; }

		const String& GetName() const { return m_name; }
		StringHash GetNameHash() const { return m_nameHash; }
		AnimationPtr GetAnimation() const { return m_animation; }
		bool IsLoop() const { return m_isLoop; }
		float GetSpeed() const { return m_speed; }

	private:
		String m_name;
		StringHash m_nameHash;
		AnimationPtr m_animation;
		HashMap<StringHash, float> m_transitions;

		bool m_isLoop{ true };
		float m_speed{ 1.0f };
	};
}