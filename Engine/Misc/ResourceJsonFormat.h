#pragma once

namespace ResourceJsonFmt
{
	struct ResourceData
	{
		std::string key;
		std::string path;
	};

	struct ShaderData
	{
		std::string key;
		std::string vsPath;
		std::string fsPath;
	};

	struct ResourceManifest
	{
		std::vector<ResourceData> textures;
		std::vector<ResourceData> cubeTextures;
		std::vector<ResourceData> models;
		std::vector<ResourceData> animations;
		std::vector<ResourceData> bgm;
		std::vector<ResourceData> sfx;
		std::vector<ShaderData>   shaders;
	};
}