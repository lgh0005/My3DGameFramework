#pragma once

namespace MGF3D
{
	class Asserter
	{
		MGF_DECLARE_UTILITIES(Asserter)

	public:
		static void _Internal_ReportFailure
		(
			cstr condition,
			cstr message,
			cstr file, int32 line
		);
	};
}
