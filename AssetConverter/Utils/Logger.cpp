#include "pch.h"
#include "Logger.h"

void Logger::Init(int argc)
{
    // 여러 싱크를 담을 벡터 생성
    std::vector<spdlog::sink_ptr> logSinks;

    // 1. 콘솔 싱크 (기존 기능)
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    // 콘솔 패턴: 색상(%^...%$) 포함
    consoleSink->set_pattern("%^[%T] [%n] %v%$");
    logSinks.push_back(consoleSink);

    // 2. 파일 싱크 (추가 기능)
    // "AssetConverter.log" 파일에 저장, true = 덮어쓰기 (실행할 때마다 새로 작성)
    // false로 하면 계속 이어쓰기(append) 됩니다. 디버깅용으로는 true가 편합니다.
    auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("AssetConverter.log", true);
    // 파일 패턴: 날짜와 로그 레벨을 명확히 표시 (색상 코드 제외)
    fileSink->set_pattern("[%Y-%m-%d %T] [%l] %v");
    logSinks.push_back(fileSink);

    // 3. 로거 생성 (싱크 목록을 전달)
    // "CONVERTER"라는 이름으로 위 두 싱크를 모두 사용하는 로거를 만듭니다.
    m_logger = std::make_shared<spdlog::logger>("CONVERTER", begin(logSinks), end(logSinks));
    spdlog::register_logger(m_logger); // (선택) 전역 로거 레지스트리에 등록

    // 4. 로그 레벨 및 플러시 설정
    m_logger->set_level(spdlog::level::trace);

    // [중요] Trace 레벨 이상의 로그가 찍힐 때마다 즉시 파일에 씁니다.
    // 프로그램이 비정상 종료(Crash)되어도 로그가 파일에 남게 하려면 필수입니다.
    m_logger->flush_on(spdlog::level::trace);

    // 5. 실행 모드 로그
    if (argc <= 1)
    {
        m_logger->info("AssetConverter build Succeeded.");
    }
    else
    {
        m_logger->info("AssetConverter Started!");
    }
}