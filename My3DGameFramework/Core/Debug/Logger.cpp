#include "CorePch.h"
#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace MGF3D
{
	Logger::SPDLoggerPtr Logger::m_logger = nullptr;

	void Logger::_Internal_Init()
	{
		if (m_logger) return;

		spdlog::set_pattern("%^[%T] %n: %v%$");
		m_logger = spdlog::stdout_color_mt("MGF3D");
		m_logger->set_level(spdlog::level::trace);
		MGF_LOG_INFO("Logger : Logger Started. [The Beginning]");
	}

	void Logger::_Internal_Flush()
	{
		if (m_logger) m_logger->flush();
	}

	void Logger::_Internal_Clear()
	{
		if (!m_logger) return;
		MGF_LOG_INFO("Logger : Logger Cleared. [The End]");
		m_logger.reset();
		spdlog::shutdown();
	}
}
