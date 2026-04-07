#pragma once

/*=====================================//
//    Delegate & Callback Macros       //
//=====================================*/
// lamda binding for member functions
#define MGF_BIND_FUNC(Method) [this]() { return this->Method(); }