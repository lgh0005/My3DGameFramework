#pragma once

namespace MGF3D
{
	enum class ELogLevel
	{
		Info,
		Warning,
		Error
	};

	struct LogMessage
	{
		ELogLevel level;
		String text;
	};

	class ConsoleManager
	{
		MGF_DECLARE_SINGLE(ConsoleManager)

	private:
		ConsoleManager();
		~ConsoleManager();

	public:
		void LogInfo(const String& message);
		void LogWarning(const String& message);
		void LogError(const String& message);
		void ClearLogs();

		const Vector<LogMessage>& GetLogs() const;

	private:
		void AddLog(ELogLevel level, const String& message);

	private:
		Vector<LogMessage> m_logs;
	};
}