#pragma once
#include "Components/Script.h"

CLASS_PTR(CameraController)
class CameraController : public Script
{
public:
    virtual ~CameraController();
    static CameraControllerUPtr Create();

public:
	virtual void Start()  override;
	virtual void Update() override;

private:
	void HandleMovement(float dt);
	void HandleRotation(float dt);

private:
    CameraController();

    float m_moveSpeed = 5.0f;
    float m_rotSpeed = 0.1f;

    float m_pitch = 0.0f;
    float m_yaw = 0.0f;

    bool m_controlEnabled = false;
    glm::vec2 m_prevMousePos{ 0.0f };
};