#include "ConverterPch.h"
#include "ParseResult.h"

namespace MGF3D
{
	bool ParseResult::IsValid() const
	{
		return mode != ConversionMode::None;
	}
}