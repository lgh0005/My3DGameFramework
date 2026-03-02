#include "CorePch.h"
#include "MGFPath.h"

namespace MGF3D
{
	MGFPath::MGFPath() = default;
	MGFPath::~MGFPath() = default;
	MGFPath::MGFPath(const fs::path& path) : m_path(path)
	{
		UpdateRawPath();
	}
	MGFPath::MGFPath(const SString& pathStr) : m_path(pathStr.CStr())
	{
		UpdateRawPath();
	}
}