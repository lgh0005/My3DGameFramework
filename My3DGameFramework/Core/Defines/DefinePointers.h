#pragma once

/*================================//
//   Class Pointer Declarations   //
//================================*/
#define MGF_CLASS_PTR(className)                                                \
    class className;                                                            \
    using className ## UPtr = MGF3D::UniquePtr<className>;                      \
    using className ## Ptr  = MGF3D::SharedPtr<className>;

/*=================================//
//   Struct Pointer Declarations   //
//=================================*/
#define MGF_STRUCT_PTR(StructName)                                              \
    struct StructName;                                                          \
    using StructName ## Ptr  = MGF3D::SharedPtr<StructName>;                    \
    using StructName ## UPtr = MGF3D::UniquePtr<StructName>;