#include "EnginePch.h"
#include "Logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

void Logger::Init()
{
	if (m_logger) return;

	spdlog::set_pattern("%^[%T] %n: %v%$");
	m_logger = spdlog::stdout_color_mt("ENGINE");
	m_logger->set_level(spdlog::level::trace);
}

void Logger::Clear()
{
	if (!m_logger) return;
	LOG_INFO("Logger : Logger Cleared. [The End]");
	m_logger.reset();
	spdlog::shutdown();
}