#include "EnginePch.h"
#include "ResourceManager.h"
#include "Graphics/StaticMesh.h"
#include "Graphics/Mesh.h"
#include "Graphics/Material.h"
#include "Graphics/Program.h"
#include "Graphics/Texture.h"
#include "Graphics/CubeTexture.h"
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

void ResourceManager::AddTexture(const std::string& name, TexturePtr texture)
{
	if (m_textures.find(name) != m_textures.end())
	{
		SPDLOG_WARN("Texture resource '{}' already exists. Overwriting.", name);
	}
	m_textures[name] = texture;
}

void ResourceManager::AddCubeTexture(const std::string& name, CubeTexturePtr texture)
{
	if (m_cubeTextures.find(name) != m_cubeTextures.end())
	{
		SPDLOG_WARN("CubeTexture resource '{}' already exists. Overwriting.", name);
	}
	m_cubeTextures[name] = texture;
}

void ResourceManager::AddModel(const std::string& name, ModelPtr model)
{
	if (m_models.find(name) != m_models.end())
	{
		SPDLOG_WARN("Model resource '{}' already exists. Overwriting.", name);
	}
	m_models[name] = model;
}

void ResourceManager::AddAnimation(const std::string& name, AnimationPtr animation)
{
	if (m_animations.find(name) != m_animations.end())
	{
		SPDLOG_WARN("Animation resource '{}' already exists. Overwriting.", name);
	}
	m_animations[name] = animation;
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
	SPDLOG_WARN("Failed to find Mesh resource: {}", name);
	return nullptr;
}

MaterialPtr ResourceManager::GetMaterial(const std::string& name) const
{
	auto it = m_materials.find(name);
	if (it != m_materials.end()) return it->second;
	SPDLOG_WARN("Failed to find Material resource: {}", name);
	return nullptr;
}

TexturePtr ResourceManager::GetTexture(const std::string& name) const
{
	auto it = m_textures.find(name);
	if (it != m_textures.end()) return it->second;
	SPDLOG_WARN("Failed to find Texture resource: {}", name);
	return nullptr;
}

CubeTexturePtr ResourceManager::GetCubeTexture(const std::string& name) const
{
	auto it = m_cubeTextures.find(name);
	if (it != m_cubeTextures.end()) return it->second;
	SPDLOG_WARN("Failed to find CubeTexture resource: {}", name);
	return nullptr;
}

ModelPtr ResourceManager::GetModel(const std::string& name) const
{
	auto it = m_models.find(name);
	if (it != m_models.end()) return it->second;
	SPDLOG_WARN("Failed to find Model resource: {}", name);
	return nullptr;
}

AnimationPtr ResourceManager::GetAnimation(const std::string& name) const
{
	auto it = m_animations.find(name);
	if (it != m_animations.end()) return it->second;
	SPDLOG_WARN("Failed to find Model resource: {}", name);
	return nullptr;
}

#pragma endregion
