#pragma once

enum class ResourceType
{
	Animation,
	// AnimChannel,
	// AnimationChannel,
	AudilClip,
	// Avatar,
	CubeTexture,
	Image,
	Material,
	Model,
	// Pose,
	// Skeleton,
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

// TODO
//enum class ResourceType
//{
//	Animation,
//	// AnimationChannel, // 보통 리소스 단위가 아니라 Animation 내부 데이터임 (확인 필요)
//	AnimationClip,
//	AudioClip,           // [Fix] AudilClip -> AudioClip
//	Avatar,
//	// Bone,             // Bone도 보통 Skeleton 내부 구조체임 (리소스 단위 아닐 수 있음)
//	CubeTexture,
//	// Image,            // Texture 생성용 CPU 데이터라면 리소스 관리 대상 아닐 수 있음
//	Material,
//	Model,
//	// Pose,             // 런타임 데이터일 가능성 높음
//	Skeleton,
//	Texture,
//	StaticMesh,
//	SkinnedMesh,
//	// ScreenMesh,       // 사각형 그리는 용도면 Mesh의 일종 (StaticMesh)으로 처리 가능
//	InstancedMesh,
//};