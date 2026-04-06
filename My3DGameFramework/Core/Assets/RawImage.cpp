#include "CorePch.h"
#include "RawImage.h"

namespace MGF3D
{
	bool RawImage::IsValid() const
	{
		return (!pixels.empty() && width > 0 && height > 0);
	}
}