#pragma once

namespace MGF3D
{
	class NameManager
	{
		MGF_DECLARE_SINGLE(NameManager)

	private:
		NameManager();
		~NameManager();

	public:
		StringHash Register(const String& name);
		StringHash RegisterUnique(const String& name);
		const String& GetString(StringHash hash) const;

	private:
		HashMap<uint64, String> m_nameTable;
		HashMap<uint64, uint32> m_nameCounters;
	};
}