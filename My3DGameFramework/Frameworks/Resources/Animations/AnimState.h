#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Animation)
#pragma endregion

CLASS_PTR(AnimState)
class AnimState
{
public:
	virtual ~AnimState();
	static AnimStateUPtr Create(const std::string& name, AnimationPtr clip);
	using TransitionMap = std::unordered_map<uint32, float>;
	void AddTransition(const std::string& targetStateName, float duration);

	void SetLoop(bool loop)	   { m_isLoop = loop; }
	void SetSpeed(float speed) { m_speed = speed; }

	float GetTransitionDuration(const std::string& targetStateName) const;
	const std::string& GetName() const { return m_name; }
	AnimationPtr GetClip()		 const { return m_clip; }
	bool IsLoop()			     const { return m_isLoop; }
	float GetSpeed()			 const { return m_speed; }

private:
	AnimState();
	void Init(const std::string& name, AnimationPtr clip);

	std::string m_name;
	AnimationPtr m_clip;

	bool m_isLoop = true;
	float m_speed = 1.0f;

	TransitionMap m_transitions;
};