#pragma once

/*======================//
//   declare singleton  //
//======================*/
#define DECLARE_SINGLE(classType)                                               \
    friend class std::unique_ptr<classType>;                                    \
    friend struct std::default_delete<classType>;                               \
                                                                                \
private:                                                                        \
    classType();                                                                \
    ~classType();                                                               \
    classType(const classType&) = delete;                                       \
    classType& operator=(const classType&) = delete;                            \
    static std::unique_ptr<classType> s_instance;                               \
                                                                                \
public:                                                                         \
    static classType* Instance()                                                \
    {                                                                           \
        return s_instance.get();                                                \
    }                                                                           \
                                                                                \
    static void Create()                                                        \
    {                                                                           \
        if (!s_instance)                                                        \
        s_instance.reset(new classType());                                      \
    }                                                                           \
                                                                                \
    static void Destroy()                                                       \
    {                                                                           \
        s_instance.reset();                                                     \
    }

/*======================//
//   define singleton   //
//======================*/
#define DEFINE_SINGLE(classType)                                                \
    std::unique_ptr<classType> classType::s_instance = nullptr;

/*=========================//
//   forward declaration   //
//=========================*/
#define FORWARD_DECLARE(className)                                               \
class className;

/*===========================//
//   declare smart pointers  //
//===========================*/
#define CLASS_PTR(className)                                                    \
class className;                                                                \
using className ## UPtr = std::unique_ptr<className>;                           \
using className ## Ptr = std::shared_ptr<className>;                            \
using className ## WPtr = std::weak_ptr<className>;