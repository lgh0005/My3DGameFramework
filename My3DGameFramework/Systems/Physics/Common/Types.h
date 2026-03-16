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
    //      Jolt Core Aliases         //
    //================================*/
    using PhysicsSystemUPtr = UniquePtr<::JPH::PhysicsSystem>;
    using TempAllocatorUPtr = UniquePtr<::JPH::TempAllocatorImpl>;
    using JobSystemUPtr     = UniquePtr<::JPH::JobSystem>;
    using JobSystemThreadPoolUPtr = UniquePtr<::JPH::JobSystemThreadPool>;

    /*================================//
    //      Common Physics Types      //
    //================================*/
    using BodyID        = ::JPH::BodyID;
    using BodyInterface = ::JPH::BodyInterface;
    
    template <typename T>
    using Ref = ::JPH::Ref<T>;

    using ShapePtr = Ref<::JPH::Shape>;
}