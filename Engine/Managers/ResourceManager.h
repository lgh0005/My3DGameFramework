#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(StaticMesh)
CLASS_PTR(Mesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
CLASS_PTR(Texture)
CLASS_PTR(CubeTexture)
CLASS_PTR(Model)
CLASS_PTR(Animation)
CLASS_PTR(AudioClip)
#pragma endregion

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager)

public:
	template<typename T>
	std::shared_ptr<T> GetResource(const std::string& name) const;

	template<typename T>
	void AddResource(const std::string& name, std::shared_ptr<T> resource);

	void Clear();

private:
	MeshPtr GetMesh(const std::string& name) const;
	MaterialPtr GetMaterial(const std::string& name) const;
	TexturePtr GetTexture(const std::string& name) const;
	CubeTexturePtr GetCubeTexture(const std::string& name) const;
	ModelPtr GetModel(const std::string& name) const;
	AnimationPtr GetAnimation(const std::string& name) const;

	void AddMesh(const std::string& name, MeshPtr mesh);
	void AddMaterial(const std::string& name, MaterialPtr material);
	void AddTexture(const std::string& name, TexturePtr texture);
	void AddCubeTexture(const std::string& name, CubeTexturePtr texture);
	void AddModel(const std::string& name, ModelPtr model);
	void AddAnimation(const std::string& name, AnimationPtr animation);

private:
	std::unordered_map<std::string, MeshPtr>	   m_meshes;
	std::unordered_map<std::string, MaterialPtr>   m_materials;
	std::unordered_map<std::string, TexturePtr>    m_textures;
	std::unordered_map<std::string, CubeTexturePtr>m_cubeTextures;
	std::unordered_map<std::string, ModelPtr>      m_models;
	std::unordered_map<std::string, AnimationPtr>  m_animations;
	std::unordered_map<std::string, AudioClipPtr>  m_audioClips;

private:
	template<typename T>
	std::unordered_map<std::string, std::shared_ptr<T>>& GetMap();

	template<typename T>
	const std::unordered_map<std::string, std::shared_ptr<T>>& GetMap() const;
};

/*====================================================//
//  Template Implementations (구현부)                 //
//====================================================*/
#pragma region TEMPLATE_IMPLEMENTATIONS

template<typename T>
inline void ResourceManager::AddResource(const std::string& name, std::shared_ptr<T> resource)
{
	// 컴파일 타임에 T에 맞는 Map을 가져옴
	auto& map = GetMap<T>();

	if (map.find(name) != map.end())
	{
		SPDLOG_WARN("Resource '{}' already exists. Overwriting.", name);
	}
	map[name] = resource;
}

template<typename T>
inline std::shared_ptr<T> ResourceManager::GetResource(const std::string& name) const
{
	// const 버전의 GetMap 호출
	const auto& map = GetMap<T>();

	auto it = map.find(name);
	if (it != map.end()) return it->second;

	SPDLOG_WARN("Failed to find resource: {}", name);
	return nullptr;
}
#pragma endregion

/*============================//
//  Template Specializations  //
//============================*/
#pragma region TEMPLATE_SPECIALIZATIONS

// 1. Mesh 연결
template<> inline std::unordered_map<std::string, MeshPtr>& ResourceManager::GetMap<Mesh>() { return m_meshes; }
template<> inline const std::unordered_map<std::string, MeshPtr>& ResourceManager::GetMap<Mesh>() const { return m_meshes; }

// 2. Material 연결
template<> inline std::unordered_map<std::string, MaterialPtr>& ResourceManager::GetMap<Material>() { return m_materials; }
template<> inline const std::unordered_map<std::string, MaterialPtr>& ResourceManager::GetMap<Material>() const { return m_materials; }

// 3. Texture 연결
template<> inline std::unordered_map<std::string, TexturePtr>& ResourceManager::GetMap<Texture>() { return m_textures; }
template<> inline const std::unordered_map<std::string, TexturePtr>& ResourceManager::GetMap<Texture>() const { return m_textures; }

// 4. CubeTexture 연결
template<> inline std::unordered_map<std::string, CubeTexturePtr>& ResourceManager::GetMap<CubeTexture>() { return m_cubeTextures; }
template<> inline const std::unordered_map<std::string, CubeTexturePtr>& ResourceManager::GetMap<CubeTexture>() const { return m_cubeTextures; }

// 5. Model 연결
template<> inline std::unordered_map<std::string, ModelPtr>& ResourceManager::GetMap<Model>() { return m_models; }
template<> inline const std::unordered_map<std::string, ModelPtr>& ResourceManager::GetMap<Model>() const { return m_models; }

// 6. Animation 연결
template<> inline std::unordered_map<std::string, AnimationPtr>& ResourceManager::GetMap<Animation>() { return m_animations; }
template<> inline const std::unordered_map<std::string, AnimationPtr>& ResourceManager::GetMap<Animation>() const { return m_animations; }

// 7. AudioClip 연결
template<> inline std::unordered_map<std::string, AudioClipPtr>& ResourceManager::GetMap<AudioClip>() { return m_audioClips; }
template<> inline const std::unordered_map<std::string, AudioClipPtr>& ResourceManager::GetMap<AudioClip>() const { return m_audioClips; }

#pragma endregion