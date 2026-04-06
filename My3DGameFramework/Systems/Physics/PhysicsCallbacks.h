#pragma once

class PhysicsCallbacks
{
	DECLARE_STATIC_CLASS(PhysicsCallbacks)

public:
	static void TraceImpl(const char* inFMT, ...);
	static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint32 inLine);
};