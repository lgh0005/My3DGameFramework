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
