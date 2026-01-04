#pragma once

class EngineConfigurator
{
	DECLARE_STATIC_CLASS(EngineConfigurator)

public:
	// 엔진 초기화 시 가장 먼저 호출 (기본값: ./Config/boot.json)
	static bool Init(const std::string& configPath = "./Config/Boot.json");

	// [Getter] 리소스 루트 경로 (예: "./Resources")
	static fs::path GetResourceRoot() { return s_resourceRoot; }
	static fs::path GetConfigRoot() { return s_configRoot; }

	// [Helper] 루트 경로와 합쳐진 '완전한 경로'를 반환
	static fs::path GetShaderConfigPath() { return s_configRoot / s_shaderConfig; }
	static fs::path GetGameConfigPath() { return s_configRoot / s_gameConfig; }

private:
	// 기본값 설정 (boot.json이 없거나 실패해도 이 값으로 작동)
	static inline fs::path s_resourceRoot = "./Resources";
	static inline fs::path s_configRoot = "./Config";
	static inline fs::path s_shaderConfig = "DefaultShaders.json";
	static inline fs::path s_gameConfig = "Resources.json";
};