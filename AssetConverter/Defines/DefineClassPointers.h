#pragma once

/*========================================//
//   declare smart pointers for classes   //
//========================================*/
#define MGF_CLASS_PTR(className)                                          \
class className;                                                          \
using className ## UPtr = UniquePtr<className>;                           \
using className ## Ptr  = SharedPtr<className>;                           \
using className ## WPtr = WeakPtr<className>;