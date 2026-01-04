#pragma once

namespace ResourceJsonFmt
{
	struct ResourceData
	{
		std::string key;
		std::string path;
	};

	struct CubeMapData
	{
		std::string key;
		std::vector<std::string> paths;
	};

	struct ShaderData
	{
		std::string key;
		std::string vsPath;
		std::string fsPath;
	};

	struct ResourceManifest
	{
		std::vector<ResourceData> bgm;
		std::vector<ResourceData> sfx;
		std::vector<ResourceData> textures;
		std::vector<CubeMapData>  cubemaps;
		std::vector<ResourceData> models;
		std::vector<ResourceData> animations;
		std::vector<ShaderData>   shaders;
		// TODO : fonts 추가 필요
	};
}