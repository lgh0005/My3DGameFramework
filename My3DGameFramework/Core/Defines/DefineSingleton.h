#pragma once

/*===========================//
//     Default Singleton     //
//===========================*/
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