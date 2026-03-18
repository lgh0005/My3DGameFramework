#pragma once

/*===========================================//
//     Subsystem initialize phrase macro     //
//===========================================*/
#define MGF_INIT_SYS(InitExpr, SystemName)                                        \
    do                                                                            \
    {                                                                             \
        if (!(InitExpr))                                                          \
        {                                                                         \
            MGF_LOG_FATAL("[SYSTEM] : Failed to initialize {0}!", SystemName);    \
            return false;                                                         \
        }                                                                         \
        MGF_LOG_TRACE("[SYSTEM] : {0} initialized successfully.", SystemName);    \
    } while(0)

/*=========================================//
//     Subsystem shutdown phrase macro     //
//=========================================*/
#define MGF_SHUTDOWN_SYS(Expr, SystemName)                                        \
    do                                                                            \
    {                                                                             \
        (Expr);                                                                   \
        MGF_LOG_TRACE("[SYSTEM] : {0} shutdown completed.", SystemName);          \
    } while(0)