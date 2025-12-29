#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

/*===================//
//   Jolt 설정 상수   //
//===================*/
namespace JoltConfig
{
    constexpr JPH::uint cMaxBodies = 65536;				// 최대 바디(물체) 수
    constexpr JPH::uint cNumBodyMutexes = 0;				// 뮤텍스 수 (0 = 기본값, 자동 설정)
    constexpr JPH::uint cMaxBodyPairs = 65536;				// 최대 바디 쌍
    constexpr JPH::uint cMaxContactConstraints = 10240;	// 최대 접촉 제약 조건
}

/*==========================================//
//   레이어 정의 (게임에서 사용할 충돌 그룹)   //
//==========================================*/
// TODO : 이후에 필요한 레이어가 있다면 추가할 필요가 있음
namespace Layers
{
    static constexpr JPH::ObjectLayer NON_MOVING = 0; // 정적 (벽, 바닥)
    static constexpr JPH::ObjectLayer MOVING = 1;     // 동적 (플레이어, 적)
    static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

namespace BroadPhaseLayers
{
    static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
    static constexpr JPH::BroadPhaseLayer MOVING(1);
    static constexpr JPH::uint NUM_LAYERS(2);
};

/*================================//
//   헬퍼 클래스 선언 (규칙 정의)   //
//================================*/
// ObjectLayer -> BroadPhaseLayer 변환기
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl();
    virtual JPH::uint GetNumBroadPhaseLayers() const override;
    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override;
#endif

private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

// BroadPhaseLayer 간 충돌 필터
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override;
};

// ObjectLayer 간 충돌 필터
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override;
};

/*=============================//
//   리스너 선언 (이벤트 수신)   //
//=============================*/
class MyBodyActivationListener : public JPH::BodyActivationListener
{
public:
    virtual void OnBodyActivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData) override;
    virtual void OnBodyDeactivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData) override;
};

class MyContactListener : public JPH::ContactListener
{
public:
    virtual JPH::ValidateResult OnContactValidate
    (
        const JPH::Body& inBody1, const JPH::Body& inBody2, 
        JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& inCollisionResult) override;

    virtual void OnContactAdded
    (
        const JPH::Body& inBody1, const JPH::Body& inBody2, 
        const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

    virtual void OnContactPersisted
    (
        const JPH::Body& inBody1, const JPH::Body& inBody2, 
        const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

    virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;
};

/*==========================//
//   Jolt 콜백 래퍼 클래스   //
//==========================*/
class PhysicsCallbacks
{
public:
    static void TraceImpl(const char* inFMT, ...);

#ifdef JPH_ENABLE_ASSERTS
    static bool AssertFailedImpl
    (
        const char* inExpression, const char* inMessage, 
        const char* inFile, uint32 inLine
    );
#endif
};