#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Mesh)
CLASS_PTR(Material)
CLASS_PTR(Program)
CLASS_PTR(Texture)
CLASS_PTR(Model)
CLASS_PTR(Animation)
#pragma endregion

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager)

public:
	template<typename T>
	std::shared_ptr<T> GetResource(const std::string& name) const;

	template<typename T>
	void AddResource(const std::string& name, std::shared_ptr<T> resource);

private:
	MeshPtr GetMesh(const std::string& name) const;
	MaterialPtr GetMaterial(const std::string& name) const;
	ProgramPtr GetProgram(const std::string& name) const;
	TexturePtr GetTexture(const std::string& name) const;
	ModelPtr GetModel(const std::string& name) const;
	AnimationPtr GetAnimation(const std::string& name) const;

	void AddMesh(const std::string& name, MeshPtr mesh);
	void AddMaterial(const std::string& name, MaterialPtr material);
	void AddProgram(const std::string& name, ProgramPtr program);
	void AddTexture(const std::string& name, TexturePtr texture);
	void AddModel(const std::string& name, ModelPtr model);
	void AddAnimation(const std::string& name, AnimationPtr animation);

private:
	std::unordered_map<std::string, MeshPtr>	  m_meshes;
	std::unordered_map<std::string, MaterialPtr>  m_materials;
	std::unordered_map<std::string, ProgramPtr>   m_programs;
	std::unordered_map<std::string, TexturePtr>   m_textures;
	std::unordered_map<std::string, ModelPtr>     m_models;
	std::unordered_map<std::string, AnimationPtr> m_animations;
};

/*==================//
//  getter inlines  //
//==================*/
#pragma region GETTER_INLINES

template<>
inline MeshPtr ResourceManager::GetResource<Mesh>(const std::string& name) const
{
	return GetMesh(name);
}

template<>
inline MaterialPtr ResourceManager::GetResource<Material>(const std::string& name) const
{
	return GetMaterial(name);
}

template<>
inline ProgramPtr ResourceManager::GetResource<Program>(const std::string& name) const
{
	return GetProgram(name);
}

template<>
inline TexturePtr ResourceManager::GetResource<Texture>(const std::string& name) const
{
	return GetTexture(name);
}

template<>
inline ModelPtr ResourceManager::GetResource<Model>(const std::string& name) const
{
	return GetModel(name);
}

template<>
inline AnimationPtr ResourceManager::GetResource<Animation>(const std::string& name) const
{
	return GetAnimation(name);
}

#pragma endregion

/*========================//
//  add resource inlines  //
//========================*/
#pragma region ADD_RESOURCE_INLINES

template<>
inline void ResourceManager::AddResource<Mesh>(const std::string& name, MeshPtr resource)
{
	AddMesh(name, resource);
}

template<>
inline void ResourceManager::AddResource<Material>(const std::string& name, MaterialPtr resource)
{
	AddMaterial(name, resource);
}

template<>
inline void ResourceManager::AddResource<Program>(const std::string& name, ProgramPtr resource)
{
	AddProgram(name, resource);
}

template<>
inline void ResourceManager::AddResource<Texture>(const std::string& name, TexturePtr resource)
{
	AddTexture(name, resource);
}

template<>
inline void ResourceManager::AddResource<Model>(const std::string& name, ModelPtr resource)
{
	AddModel(name, resource);
}

template<>
inline void ResourceManager::AddResource<Animation>(const std::string& name, AnimationPtr resource)
{
	AddAnimation(name, resource);
}

#pragma endregion