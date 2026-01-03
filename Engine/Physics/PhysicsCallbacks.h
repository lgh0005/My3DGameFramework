#pragma once

class PhysicsCallbacks
{
public:
	static void TraceImpl(const char* inFMT, ...);

#ifdef JPH_ENABLE_ASSERTS
	static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint32 inLine);
#endif
};