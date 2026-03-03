#pragma once

/*===================================//
//     Default Class Constraints     //
//===================================*/
// Copy constructor & assignment blocking
#define MGF_DISABLE_COPY(classType)                                             \
    classType(const classType&) = delete;                                       \
    classType& operator=(const classType&) = delete;

// Move semantic blocking
#define MGF_DISABLE_MOVE(classType)                                             \
    classType(classType&&) = delete;                                            \
    classType& operator=(classType&&) = delete;

// Static methods only (Utility classes)
#define MGF_DECLARE_UTILITIES(classType)                                        \
private:                                                                        \
    classType() = delete;                                                       \
    ~classType() = delete;                                                      \
    MGF_DISABLE_COPY(classType)                                                 \
    MGF_DISABLE_MOVE(classType)

// Singleton declaration (Meyers' Singleton)
#define MGF_DECLARE_SINGLE(classType)                                           \
private:                                                                        \
    MGF_DISABLE_COPY(classType)                                                 \
    MGF_DISABLE_MOVE(classType)                                                 \
                                                                                \
public:                                                                         \
    static classType& Instance()                                                \
    {                                                                           \
        static classType s_instance;                                            \
        return s_instance;                                                      \
    }

/*================================//
//   Class Pointer Declarations   //
//================================*/
#define MGF_CLASS_PTR(className)                                                \
class className;                                                                \
using className ## UPtr = std::unique_ptr<className>;                           \
using className ## Ptr = std::shared_ptr<className>;                            \
using className ## WPtr = std::weak_ptr<className>;

/*=============================//
//   Default Engine Asserter   //
//=============================*/
#define MGF_STATIC_ASSERT(condition, message) static_assert(condition, message)


/*===================================//
//      Synchronization Helpers      //
//===================================*/
#define MGF_LOCK_SCOPE