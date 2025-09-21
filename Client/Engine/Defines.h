#pragma once

/*================
//    Macros    //
================*/

// Make singleton class
#define DECLARE_SINGLE(classname)                                                \
private:                                                                         \
    classname() = default;                                                       \
    friend class ISingleton<classname>;
