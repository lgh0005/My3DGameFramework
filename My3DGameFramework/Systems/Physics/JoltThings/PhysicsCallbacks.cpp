#include "EnginePch.h"
#include "Physics/PhysicsCallbacks.h"

void PhysicsCallbacks::TraceImpl(const char* inFMT, ...)
{
	// 가변 인자 처리
	va_list list;
	va_start(list, inFMT);

	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);

	va_end(list);

	// 2. 완성된 문자열을 엔진 로거로 출력
	LOG_INFO("Jolt: {}", buffer);
}

bool PhysicsCallbacks::AssertFailedImpl
(
	const char* inExpression, 
	const char* inMessage, 
	const char* inFile, 
	uint32 inLine
)
{
	LOG_ERROR
	(
		"Jolt Assert: {}:{}: ({}) {}", 
		inFile, 
		inLine, 
		inExpression, 
		inMessage ? inMessage : ""
	);
	return true;
}