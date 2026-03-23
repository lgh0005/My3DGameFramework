#pragma once

#pragma region JOLT_FORWARD_DECLARATIONS
namespace JPH
{
    class PhysicsSystem;
    class TempAllocatorImpl;
    class JobSystem;
    class JobSystemThreadPool;
    class BodyInterface;
    class BodyID;
    class Shape;
    template <class T> class Ref;
}
#pragma endregion

namespace MGF3D
{
    /*================================//
    //      Common Physics Types      //
    //================================*/
    template<typename T> using Ref = ::JPH::Ref<T>;
    using BodyID        = ::JPH::BodyID;
    using BodyInterface = ::JPH::BodyInterface;

    /*================================//
    //      Jolt Core Aliases         //
    //================================*/
    using ShapePtr                = Ref<::JPH::Shape>;
    using PhysicsMaterialPtr      = Ref<::JPH::PhysicsMaterial>;
    using GroupFilterPtr          = Ref<::JPH::GroupFilter>;

    using JobSystemUPtr           = UniquePtr<::JPH::JobSystem>;
    using PhysicsSystemUPtr       = UniquePtr<::JPH::PhysicsSystem>;
    using JobSystemThreadPoolUPtr = UniquePtr<::JPH::JobSystemThreadPool>;
    using TempAllocatorUPtr       = UniquePtr<::JPH::TempAllocatorImpl>;
}