#pragma once

/*===============================//
//     Default Utility class     //
//===============================*/
// Define utility class with deleted constructor and destructor, and copy/move blocking.
#define MGF_DECLARE_UTILITIES(classType)                                        \
private:                                                                        \
    classType() = delete;                                                       \
    ~classType() = delete;                                                      \
    MGF_DISABLE_COPY(classType)                                                 \
    MGF_DISABLE_MOVE(classType)