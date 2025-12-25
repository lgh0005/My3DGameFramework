#pragma once

/*======================//
//   declare singleton  //
//======================*/
#define DECLARE_SINGLE(classType)                                               \
private:                                                                        \
    classType() = default;                                                      \
    ~classType() = default;                                                     \
                                                                                \
    classType(const classType&) = delete;                                       \
    classType& operator=(const classType&) = delete;                            \
    classType(classType&&) = delete;                                            \
    classType& operator=(classType&&) = delete;                                 \
                                                                                \
public:                                                                         \
    static classType& Instance()                                                \
    {                                                                           \
        static classType s_instance;                                            \
        return s_instance;                                                      \
    }

/*===========================//
//   declare smart pointers  //
//===========================*/
#define CLASS_PTR(className)                                                    \
class className;                                                                \
using className ## UPtr = std::unique_ptr<className>;                           \
using className ## Ptr = std::shared_ptr<className>;                            \
using className ## WPtr = std::weak_ptr<className>;

/*==============================================//
//   ubo binding numbers and max light counts   //
//==============================================*/
// ubo binding number macro
#define UBO_POINT_CAMERA 0
#define UBO_POINT_LIGHT  1

// max light and shadow caster count per scene
#define MAX_LIGHTS 32
#define MAX_SHADOW_CASTER 8

/*===============================//
//   Jolt physics const macros   //
//===============================*/
// max memory allocation value
#define MAX_PHYSICS_MEMORY_ALLOC_VALUE 10 * 1024 * 1024 // 10MB