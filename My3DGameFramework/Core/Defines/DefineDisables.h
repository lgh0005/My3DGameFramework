#pragma once

/*===================================//
//     Default Class Constraints     //
//===================================*/
// Copy constructor & assignment blocking.
#define MGF_DISABLE_COPY(classType)                                             \
    classType(const classType&) = delete;                                       \
    classType& operator=(const classType&) = delete;

// Move semantic blocking.
#define MGF_DISABLE_MOVE(classType)                                             \
    classType(classType&&) = delete;                                            \
    classType& operator=(classType&&) = delete;