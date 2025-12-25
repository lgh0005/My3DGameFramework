#pragma once
#include "Component.h"
#include <Jolt/Jolt.h>

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
    void SyncPhysicsToTransform(); // P -> T (Update에서 호출)
    void SyncTransformToPhysics(); // T -> P (Start나 텔레포트 시 호출)
    JPH::BodyInterface& GetBodyInterface() const;

    JPH::BodyID m_bodyID;
};