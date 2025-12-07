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

/*==========================================================//
//   declare application creation for application derives   //
//==========================================================*/
// Defines a static 'Create' function that enforces the mandatory Init() call sequence.
// Ensures safe instantiation by validating the result of Init() before returning.
#define DECLARE_APPLICATION_CREATE(className)                                  \
public:                                                                        \
    static std::unique_ptr<className> Create                                   \
    (int32 width, int32 height, const std::string& title)                      \
    {                                                                          \
        auto app = std::unique_ptr<className>(new className());                \
        if (!app->Init(width, height, title)) return nullptr;                  \
        return app;                                                            \
    }