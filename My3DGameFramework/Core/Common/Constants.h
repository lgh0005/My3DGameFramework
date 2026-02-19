#pragma once

namespace Constants
{
	// JSON Config Schema
	constexpr std::string_view KEY_DESCRIPTION = "Description";
	constexpr std::string_view KEY_TOKENS = "Tokens";
	constexpr std::string_view KEY_MODULES = "Modules";

	// YAML Schema
	constexpr std::string_view ROOT_MATERIAL = "Material";

	// Internal Tokens : 엔진 내부에서 사용하는 식별자
	namespace MatTokens
	{
		// 1. Texture Tokens
		constexpr std::string_view TEX_DIFFUSE = "TEX_DIFFUSE";
		constexpr std::string_view TEX_NORMAL = "TEX_NORMAL";
		constexpr std::string_view TEX_METALLIC = "TEX_METALLIC";
		constexpr std::string_view TEX_ROUGHNESS = "TEX_ROUGHNESS";
		constexpr std::string_view TEX_AO = "TEX_AO";
		constexpr std::string_view TEX_EMISSION = "TEX_EMISSION";
		constexpr std::string_view TEX_ORM = "TEX_ORM";
		constexpr std::string_view TEX_HEIGHT = "TEX_HEIGHT";
		constexpr std::string_view TEX_SPECULAR = "TEX_SPECULAR";

		// 2. Factor/Value Tokens
		constexpr std::string_view VAL_ALBEDO_COLOR = "VAL_ALBEDO_COLOR";
		constexpr std::string_view VAL_EMISSIVE_COLOR = "VAL_EMISSIVE_COLOR";
		constexpr std::string_view VAL_METALLIC = "VAL_METALLIC";
		constexpr std::string_view VAL_ROUGHNESS = "VAL_ROUGHNESS";
		constexpr std::string_view VAL_EMISSION_STRENGTH = "VAL_EMISSION_STRENGTH";
		constexpr std::string_view VAL_SHININESS = "VAL_SHININESS";
		constexpr std::string_view VAL_HEIGHT_SCALE = "VAL_HEIGHT_SCALE";

		// 3. Flag Tokens
		constexpr std::string_view FLAG_USE_GLOSSINESS = "FLAG_USE_GLOSSINESS";
	}
}