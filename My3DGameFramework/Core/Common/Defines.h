#pragma once

/*===================================//
//     Default Class Constraints     //
//===================================*/
// Copy constructor & assignment blocking
#define DISABLE_COPY(classType)                                                 \
    classType(const classType&) = delete;                                       \
    classType& operator=(const classType&) = delete;

// Move semantic blocking
#define DISABLE_MOVE(classType)                                                 \
    classType(classType&&) = delete;                                            \
    classType& operator=(classType&&) = delete;

// Static methods only (Utility classes)
#define DECLARE_UTILITIES(classType)                                            \
private:                                                                        \
    classType() = delete;                                                       \
    ~classType() = delete;                                                      \
    DISABLE_COPY(classType)                                                     \
    DISABLE_MOVE(classType)

// Singleton declaration (Meyers' Singleton)
#define DECLARE_SINGLE(classType)                                               \
private:                                                                        \
    DISABLE_COPY(classType)                                                     \
    DISABLE_MOVE(classType)                                                     \
                                                                                \
public:                                                                         \
    static classType& Instance()                                                \
    {                                                                           \
        static classType s_instance;                                            \
        return s_instance;                                                      \
    }

/*==============================//
//   Class Pointer Declarations //
//==============================*/
#define CLASS_PTR(className)                                                    \
class className;                                                                \
using className ## UPtr = std::unique_ptr<className>;                           \
using className ## Ptr = std::shared_ptr<className>;                            \
using className ## WPtr = std::weak_ptr<className>;

