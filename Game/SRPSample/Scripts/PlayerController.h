#pragma once
#include "Components/Script.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Animator)
CLASS_PTR(AnimController)
#pragma endregion

CLASS_PTR(PlayerController)
class PlayerController : public Script
{
public:
    virtual ~PlayerController();
    static PlayerControllerUPtr Create();

public:
    virtual void Start()  override;
    virtual void Update() override;

private:
    void HandleMovement(float dt);
    void UpdateFiniteStateMachine();
    void ActiveSelfTest();

private:
    PlayerController();

    bool m_isMoving = false;
    float m_moveSpeed = 2.0f;
    Animator* m_animator;
    AnimController* m_animController;
};