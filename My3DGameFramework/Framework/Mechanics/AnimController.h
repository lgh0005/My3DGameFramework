#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Animation)
CLASS_PTR(AnimState)
CLASS_PTR(Pose)
CLASS_PTR(Skeleton)
#pragma endregion

CLASS_PTR(AnimController)
class AnimController
{
public:
	virtual ~AnimController();
	static AnimControllerUPtr Create();
	using AnimStateMap = std::unordered_map<std::string, AnimStatePtr>;
	void AddState(const std::string& name, AnimationPtr clip, bool isLoop = true, float speed = 1.0f);
	void CrossFade(const std::string& stateName, float duration = -1.0f);
	void Update(float deltaTime);
	void Play(const std::string& stateName);

	void SetTransitionDuration(const std::string& from, const std::string& to, float duration);
	void SetStartState(const std::string& name);
	Pose GetPose(uint32 nodeNameHash, const Pose& defaultPose) const;

private:
	AnimController();
	void TransitTo(std::shared_ptr<AnimState> nextState, float duration);
	AnimStateMap m_states;

	// 현재/다음 상태 (포인터로 가리킴)
	AnimStatePtr m_currentState;
	AnimStatePtr m_nextState;

	// 시간 관리
	float m_currentTime = 0.0f;
    float m_nextTime = 0.0f;

	// 블렌딩 제어
    bool  m_isBlending = false;
    float m_blendTimer = 0.0f;
    float m_blendDuration = 0.0f;
	float m_finalBlendFactor = 0.0f;
};