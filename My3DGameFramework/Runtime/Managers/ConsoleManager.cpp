#include "RuntimePch.h"
#include "ConsoleManager.h"

namespace MGF3D
{
	ConsoleManager::ConsoleManager() = default;
	ConsoleManager::~ConsoleManager() = default;

	void ConsoleManager::LogInfo(const String& message)
	{
		AddLog(ELogLevel::Info, message);
	}

	void ConsoleManager::LogWarning(const String& message)
	{
		AddLog(ELogLevel::Warning, message);
	}

	void ConsoleManager::LogError(const String& message)
	{
		AddLog(ELogLevel::Error, message);
	}

	void ConsoleManager::ClearLogs()
	{
		m_logs.clear();
	}

	const Vector<LogMessage>& ConsoleManager::GetLogs() const
	{
		return m_logs;
	}

	void ConsoleManager::AddLog(ELogLevel level, const String& message)
	{
		if (m_logs.size() >= 1000) m_logs.erase(m_logs.begin());
		m_logs.push_back({ level, message });
	}
}