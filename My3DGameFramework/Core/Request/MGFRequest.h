#pragma once

namespace MGF3D
{
	struct MGFRequest
	{
		void** data;
		Atomic<bool>* isReady;
	};
}