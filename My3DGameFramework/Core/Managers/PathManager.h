#pragma once
#include "Thread/MGFSignal.h"
#include "Hashing/StringHash.h"

namespace MGF3D
{
	class PathManager
	{
		MGF_DECLARE_SINGLE(PathManager)

	private:
		PathManager();
		~PathManager();

	public:
		bool Init();
		void Shutdown();

	public:
		bool LoadConfig(const MGFPath& configFileName = "EngineConfig.json");
		void AddVirtualPath(const MGFName& alias, const MGFPath& actualPath);
		MGFPath Resolve(const MGFPath& virtualPath) const;
		void WaitForConfig();

		// [DEBUG]
		void DebugDumpMap();

	private:
		MGFPath m_engineRoot;
		SMap<StringHash, MGFPath> m_virtualPaths;
		mutable Mutex m_pathMutex;
		MGFSignal m_configSignal;
	};
}