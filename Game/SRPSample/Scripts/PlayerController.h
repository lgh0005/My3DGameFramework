#pragma once
#include "Components/Script.h"

#pragma region FORWARD_DECLARATION
CLASS_PTR(Animator)
CLASS_PTR(AnimController)
CLASS_PTR(Rigidbody)
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
    virtual void FixedUpdate() override;

private:
    void HandleMovement(float dt);
    void UpdateFiniteStateMachine();
    void ActiveSelfTest();

private:
    PlayerController();

    bool m_isMoving = false;
    float m_moveSpeed = 2.0f;
    glm::vec3 m_moveDir = glm::vec3(1.0f);

    Rigidbody* m_rigidbody;

    Animator* m_animator;
    AnimController* m_animController;
};