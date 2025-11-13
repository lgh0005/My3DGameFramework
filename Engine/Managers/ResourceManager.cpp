#include "EnginePch.h"
#include "ResourceManager.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/Program.h"
#include "Graphics/Texture.h"
#include "Graphics/Model.h"
#include "Graphics/Animation.h"

// TODO : 이후에 음원과 관련된 리소스들도 추가 필요

/*========================//
//  add resource methods  //
//========================*/
#pragma region ADD_RESOURCE_METHODS

void ResourceManager::AddMesh(const std::string& name, MeshPtr mesh)
{
	if (m_meshes.find(name) != m_meshes.end())
	{
		SPDLOG_WARN("Mesh resource '{}' already exists. Overwriting.", name);
	}
	m_meshes[name] = mesh;
}

void ResourceManager::AddMaterial(const std::string& name, MaterialPtr material)
{
	if (m_materials.find(name) != m_materials.end())
	{
		SPDLOG_WARN("Material resource '{}' already exists. Overwriting.", name);
	}
	m_materials[name] = material;
}

void ResourceManager::AddProgram(const std::string& name, ProgramPtr program)
{
	if (m_programs.find(name) != m_programs.end())
	{
		SPDLOG_WARN("Program resource '{}' already exists. Overwriting.", name);
	}
	m_programs[name] = program;
}

void ResourceManager::AddTexture(const std::string& name, TexturePtr texture)
{
	if (m_textures.find(name) != m_textures.end())
	{
		SPDLOG_WARN("Texture resource '{}' already exists. Overwriting.", name);
	}
	m_textures[name] = texture;
}

#pragma endregion

/*==================//
//  getter methods  //
//==================*/
#pragma region GETTER_METHODS

MeshPtr ResourceManager::GetMesh(const std::string& name) const
{
	auto it = m_meshes.find(name);
	if (it != m_meshes.end()) return it->second;
	SPDLOG_ERROR("Failed to find Mesh resource: {}", name);
	return nullptr;
}

MaterialPtr ResourceManager::GetMaterial(const std::string& name) const
{
	auto it = m_materials.find(name);
	if (it != m_materials.end()) return it->second;
	SPDLOG_ERROR("Failed to find Material resource: {}", name);
	return nullptr;
}

ProgramPtr ResourceManager::GetProgram(const std::string& name) const
{
	auto it = m_programs.find(name);
	if (it != m_programs.end()) return it->second;
	SPDLOG_ERROR("Failed to find Program resource: {}", name);
	return nullptr;
}

TexturePtr ResourceManager::GetTexture(const std::string& name) const
{
	auto it = m_textures.find(name);
	if (it != m_textures.end()) return it->second;
	SPDLOG_ERROR("Failed to find Texture resource: {}", name);
	return nullptr;
}

ModelPtr ResourceManager::GetModel(const std::string& name) const
{
	auto it = m_models.find(name);
	if (it != m_models.end()) return it->second;
	SPDLOG_ERROR("Failed to find Model resource: {}", name);
	return nullptr;
}

AnimationPtr ResourceManager::GetAnimation(const std::string& name) const
{
	auto it = m_animations.find(name);
	if (it != m_animations.end()) return it->second;
	SPDLOG_ERROR("Failed to find Model resource: {}", name);
	return nullptr;
}

#pragma endregion

