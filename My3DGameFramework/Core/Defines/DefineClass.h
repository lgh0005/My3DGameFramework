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