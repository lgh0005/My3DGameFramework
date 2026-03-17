/*========================================//
//     Module initialize phrase macro     //
//========================================*/
#define MGF_MODULE_INIT(moduleExpr, moduleName)                                            \
    do                                                                                     \
    {                                                                                      \
        MGF_LOG_INFO("[BOOT] Initializing {0}...", moduleName);                            \
        if (!(moduleExpr))                                                                 \
        {                                                                                  \
            MGF_LOG_FATAL("[BOOT] Critical Error: Failed to initialize {0}!", moduleName); \
            return false;                                                                  \
        }                                                                                  \
        MGF_LOG_INFO("[BOOT] {0} booted successfully.", moduleName);                       \
    } while(0)

/*======================================//
//     Module shutdown phrase macro     //
//======================================*/
#define MGF_MODULE_SHUTDOWN(moduleExpr, moduleName)                                        \
    do                                                                                     \
    {                                                                                      \
        MGF_LOG_INFO("[EXIT] Shutting down {0}...", moduleName);                           \
        moduleExpr;                                                                        \
    } while(0)