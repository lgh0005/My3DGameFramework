#include "PhysicsPch.h"
#include "JoltDiagnostic.h"
#include <cstdarg>

namespace MGF3D
{
    void JoltDiagnostic::Bind()
    {
        MGF_JPH_TRACE_BIND(OnJoltTrace);
        MGF_JPH_ASSERT_BIND(OnJoltAssertFailed);
    }

    void JoltDiagnostic::OnJoltTrace(cstr inFMT, ...)
    {
        va_list args;

        va_start(args, inFMT);
        char8 buffer[1024];
        vsnprintf(buffer, sizeof(buffer), inFMT, args);
        va_end(args);

        MGF_LOG_INFO("[Jolt] : {0}", buffer);
    }

    bool JoltDiagnostic::OnJoltAssertFailed
    (
        cstr inExpression,
        cstr inMessage,
        cstr inFile,
        uint32 inLine
    )
    {
        Asserter::_Internal_ReportFailure
        (
            inExpression,
            CommonUtils::Select<cstr>(inMessage, inMessage, "No Message"),
            inFile,
            static_cast<int32>(inLine)
        );

        MGF_PLATFORM_BREAK();
        return false;
    }
}