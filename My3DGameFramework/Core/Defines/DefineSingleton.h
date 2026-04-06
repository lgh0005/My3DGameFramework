#pragma once
#include "Defines/DefineDisables.h"

/*===========================//
//     Default Singleton     //
//===========================*/
// Define a singleton class with meyers singleton pattern.
#define MGF_DECLARE_SINGLE(classType)                                           \
private:                                                                        \
    MGF_DISABLE_COPY(classType)                                                 \
    MGF_DISABLE_MOVE(classType)                                                 \
                                                                                \
public:                                                                         \
    static classType& Instance()                                                \
    {                                                                           \
        static classType* s_instance = new classType();                         \
        return *s_instance;                                                     \
    }