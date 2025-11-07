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

/*======================//
//   define singleton   //
//======================*/
#define DEFINE_SINGLE(classType)                                                \
    std::unique_ptr<classType> classType::s_instance = nullptr;

/*===========================//
//   declare smart pointers  //
//===========================*/
#define CLASS_PTR(className)                                                    \
class className;                                                                \
using className ## UPtr = std::unique_ptr<className>;                           \
using className ## Ptr = std::shared_ptr<className>;                            \
using className ## WPtr = std::weak_ptr<className>;