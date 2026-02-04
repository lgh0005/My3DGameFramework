#pragma once

namespace EnginePolicy
{
	// 물리적 경로 정책 (Boot Strap)
	// 유일하게 허용되는 하드코딩된 물리 경로들
	namespace Disk
	{
		static constexpr std::string_view BootConfigPath = "./Config/EngineConfig.json";
	}

	// JSON 스키마 키 (EngineConfig.json)
	// 오타 방지를 위해 키값들을 상수로 박제
	namespace Schema
	{
		static constexpr std::string_view SettingsSection = "settings";
		static constexpr std::string_view KeyAssetRoot = "assetRoot";
		static constexpr std::string_view KeyConfigRoot = "configRoot";
		static constexpr std::string_view KeyEngineDir = "engineDir";
		static constexpr std::string_view KeyGameDir = "gameDir";
	}

	// 가상 경로 접두사 (Virtual Paths)
	// 코드에서 사용할 @키워드들
	namespace Path
	{
		static constexpr std::string_view PrefixEngine = "@Engine";
		static constexpr std::string_view PrefixGame = "@Game";
		static constexpr std::string_view PrefixAssets = "@Assets";

		static constexpr std::string_view PrefixConfig = "@Config";
		static constexpr std::string_view PrefixEngineConfig = "@EngineConfig";
		static constexpr std::string_view PrefixGameConfig = "@GameConfig";
	}
}