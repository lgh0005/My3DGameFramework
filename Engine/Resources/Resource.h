#pragma once

enum class ResourceType
{
	Animation,
	AudilClip,
	CubeTexture,
	Image,
	Material,
	Model,
	Texture,
	StaticMesh,
	SkinnedMesh,
	ScreenMesh,
	InstancedMesh,
};

CLASS_PTR(Resource)
class Resource
{
public:
	virtual ~Resource() = default;
	virtual ResourceType GetResourceType() const = 0;
	bool MatchesType(ResourceType type) const;

	const std::string& GetPath() const { return m_path; }
	void SetPath(const std::string& path) { m_path = path; }

	const std::string& GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

protected:
	std::string m_path;
	std::string m_name;
};