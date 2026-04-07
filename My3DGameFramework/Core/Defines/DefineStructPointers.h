#pragma once

/*========================================//
//   declare smart pointers for structs   //
//========================================*/
#define MGF_STRUCT_PTR(structName)                                         \
struct structName;                                                         \
using  structName ## UPtr = UniquePtr<structName>;                         \
using  structName ## Ptr  = SharedPtr<structName>;                         \
using  structName ## WPtr = WeakPtr<structName>;