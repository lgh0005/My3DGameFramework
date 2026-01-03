#pragma once
#include "Component.h"

#pragma region FORWARD_DECLARATION
namespace JPH { class BodyInterface; }
#pragma endregion

class PhysicsComponent : public Component
{
public:
    virtual ~PhysicsComponent();

    bool IsValid() const { return !m_bodyID.IsInvalid(); }
    JPH::BodyID GetBodyID() const { return m_bodyID; }

    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::quat& rotation);

protected:
    virtual void OnEnable() override;   // 켜질 때 -> 바디 생성
    virtual void OnDisable() override;  // 꺼질 때 -> 바디 제거
    virtual void OnDestroy() override;  // 삭제될 때 -> 바디 제거 (안전장치)
    virtual void Update() override;     // 매 프레임 -> 위치 동기화 (P -> T)

    virtual void CreateBody();
    void DestroyBody();

    void SyncPhysicsToTransform();      // P -> T (Update에서 호출)
    void SyncTransformToPhysics();      // T -> P (Start나 텔레포트 시 호출)
    JPH::BodyInterface& GetBodyInterface() const;

protected:
    PhysicsComponent();
    JPH::BodyID m_bodyID;
};