#pragma once

/*==================================//
//    default static assert macro   //
//==================================*/
#define MGF_STATIC_ASSERT(condition, message) static_assert(condition, message)

/*==============================//
//    default assertion macro   //
//==============================*/
#ifdef _DEBUG
    #define MGF_ASSERT(condition, message)														   \
        do {																					   \
            if (!(!!(condition)))																   \
            {																					   \
                MGF3D::Asserter::_Internal_ReportFailure(#condition, message, __FILE__, __LINE__); \
                MGF_PLATFORM_BREAK();															   \
            }																					   \
        } while (0)
#else
    #define MGF_ASSERT(condition, message) ((void)0)
#endif