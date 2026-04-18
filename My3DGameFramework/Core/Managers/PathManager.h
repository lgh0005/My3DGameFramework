#pragma once

namespace MGF3D
{
	class PathManager
	{
		MGF_DECLARE_SINGLE(PathManager)

	private:
		PathManager();
		~PathManager();

	public:
		bool Init(const String& configPath);
		String ResolvePath(const String& virtualPath) const;
		void RegisterVirtualPath(const String& alias, const String& actualPath);

	private:
		mutable Mutex m_pathMutex;
		HashMap<StringHash, String> m_virtualPaths;
	};
}

#include "Managers/PathManager.inl"