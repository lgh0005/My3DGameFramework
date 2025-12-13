#include "pch.h"
#include "Logger.h"

void Logger::Init()
{
	// 1. 로그 패턴 설정
	// %^: 색상 시작, %T: 시간, %n: 로거 이름, %v: 메시지, %$: 색상 끝
	spdlog::set_pattern("%^[%T] [%n] %v%$");

	// 2. 로거 생성 (이름: CONVERTER)
	m_logger = spdlog::stdout_color_mt("CONVERTER");

	// 3. 로그 레벨 설정 (Trace로 해두면 모든 로그가 다 보임)
	m_logger->set_level(spdlog::level::trace);

	// 초기화 확인 로그
	m_logger->info("Logger Initialized.");
}