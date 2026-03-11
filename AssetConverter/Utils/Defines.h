#pragma once

/*===================================//
//     default class constraints     //
//===================================*/
// Copy constructor & assignment blocking
#define MGF_DISABLE_COPY(classType)                                             \
    classType(const classType&) = delete;                                       \
    classType& operator=(const classType&) = delete;

// Move semantic blocking
#define MGF_DISABLE_MOVE(classType)                                             \
    classType(classType&&) = delete;                                            \
    classType& operator=(classType&&) = delete;

/*======================//
//   declare singleton  //
//======================*/
#define DECLARE_SINGLE(classType)                                               \
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

/*==========================//
//   declare utility class  //
//==========================*/
#define MGF_DECLARE_UTILITIES(classType)                                        \
private:                                                                        \
    classType() = delete;                                                       \
    ~classType() = delete;                                                      \
    MGF_DISABLE_COPY(classType)                                                 \
    MGF_DISABLE_MOVE(classType)