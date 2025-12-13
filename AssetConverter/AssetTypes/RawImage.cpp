#include "pch.h"
#include "RawImage.h"

bool RawImage::IsValid() const
{
	return (!pixels.empty() && width > 0 && height > 0);
}