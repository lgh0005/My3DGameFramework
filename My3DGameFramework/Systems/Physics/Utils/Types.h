#pragma once

namespace JPH
{
    class PhysicsSystem;
    class TempAllocatorImpl;
    class JobSystem;
    class BodyInterface;
    class BodyID;
    class Shape;
    template <class T> class Ref;
}

namespace MGF3D
{
    /*================================//
    //      Jolt Core Aliases         //
    //================================*/
    using PhysicsSystemUPtr     = UniquePtr<::JPH::PhysicsSystem>;
    using TempAllocatorImplUPtr = UniquePtr<::JPH::TempAllocatorImpl>;
    using JobSystemUPtr         = UniquePtr<::JPH::JobSystem>;

    /*================================//
    //      Common Physics Types      //
    //================================*/
    using BodyID        = ::JPH::BodyID;
    using BodyInterface = ::JPH::BodyInterface;
    
    template <typename T>
    using Ref = ::JPH::Ref<T>;

    using ShapePtr = Ref<::JPH::Shape>;
}