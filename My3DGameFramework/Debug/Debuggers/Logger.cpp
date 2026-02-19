#include "DebugPch.h"
#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace MGF3D
{
	Logger::SPDLoggerPtr Logger::m_logger = nullptr;
	Logger::Logger() = default;
	Logger::~Logger() = default;

	void Logger::Init()
	{
		if (m_logger) return;

		spdlog::set_pattern("%^[%T] %n: %v%$");
		m_logger = spdlog::stdout_color_mt("MGF3D");
		m_logger->set_level(spdlog::level::trace);
		LOG_INFO("Logger : Logger Started. [The Beginning]");
	}

	void Logger::Flush()
	{
		if (m_logger) m_logger->flush();
	}

	void Logger::Clear()
	{
		if (!m_logger) return;
		LOG_INFO("Logger : Logger Cleared. [The End]");
		m_logger.reset();
		spdlog::shutdown();
	}
}
