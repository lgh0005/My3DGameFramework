#include "CorePch.h"
#include "FilePath.h"

namespace MGF3D
{
	FilePath::FilePath() = default;
	FilePath::~FilePath() = default;
	FilePath::FilePath(const fs::path& path) : m_path(path)
	{
		UpdateRawPath();
	}
}