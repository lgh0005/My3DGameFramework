#pragma once
#include <Jolt/Physics/Body/MotionType.h>

CLASS_PTR(RigidbodyConstraint)
class RigidbodyConstraint
{
public:
    enum Type : uint8
    {
        None = 0,
        FreezePositionX = 1 << 0,
        FreezePositionY = 1 << 1,
        FreezePositionZ = 1 << 2,
        FreezeRotationX = 1 << 3,
        FreezeRotationY = 1 << 4,
        FreezeRotationZ = 1 << 5,

        FreezePosition = FreezePositionX | FreezePositionY | FreezePositionZ,
        FreezeRotation = FreezeRotationX | FreezeRotationY | FreezeRotationZ,
        FreezeAll      = FreezePosition | FreezeRotation
    };

    // 생성자
    RigidbodyConstraint();
    RigidbodyConstraint(uint8 mask);
    RigidbodyConstraint(Type type);

    // 유틸리티 함수
    void Set(Type flag) { m_bitMask |= flag; }
    void Unset(Type flag) { m_bitMask &= ~flag; }
    bool IsSet(Type flag) const { return (m_bitMask & flag) != 0; }

    // 연산자 오버로딩 (편의성)
    RigidbodyConstraint operator|(Type flag) const;
    RigidbodyConstraint operator|(const RigidbodyConstraint& other) const;
    void                operator|=(Type flag);
    bool operator==(const RigidbodyConstraint& other) const;
    bool operator!=(const RigidbodyConstraint& other) const;

    // 마스크 getter와 setter
    uint8 GetBitMask() const { return m_bitMask; }
    void SetBitMask(uint8 mask) { m_bitMask = mask; }

private:
    uint8 m_bitMask = None;
};