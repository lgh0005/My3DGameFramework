#pragma once
#include <cmath>
#include <algorithm>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/constants.hpp>

#pragma region FORWARD_DECLARATION
CLASS_PTR(StaticMesh)
#pragma endregion

/*===========//
//   Plane   //
//===========*/
class CullingPlane
{
public:
	CullingPlane();
	~CullingPlane();
	static CullingPlane Create(const glm::vec4& eq);

	void Normalize();
	float GetDistanceToPoint(const glm::vec3& point) const;

	const glm::vec3& Normal() const { return m_normal; }
	float NormalX() const { return m_normal.x; }
	float NormalY() const { return m_normal.y; }
	float NormalZ() const { return m_normal.z; }
	float Distance() const { return m_distance; }

private:
	CullingPlane(const glm::vec3& normal, float dist);
	glm::vec3 m_normal;
	float     m_distance;
};

/*=============================================//
//   frustum culling : AABB for MeshRenderer   //
//=============================================*/
class RenderBounds
{
public:
	~RenderBounds();
	static RenderBounds Empty();
	static RenderBounds Create(const glm::vec3& center, const glm::vec3& extents);
	static RenderBounds CreateFromMinMax(const glm::vec3& min, const glm::vec3& max);
	
	RenderBounds Transform(const glm::mat4& mat) const;
	RenderBounds Union(const RenderBounds& other) const;
	const glm::vec3& GetCenter() const { return m_center; }
	const glm::vec3& GetExtents() const { return m_extents; }

private:
	RenderBounds();
	glm::vec3 m_center	{ 0.0f };
	glm::vec3 m_extents { 0.0f };
};

/*=========================//
//   geometry generator    //
//=========================*/
class GeometryGenerator
{
public:
	static StaticMeshUPtr CreateBox();
	static StaticMeshUPtr CreatePlane();
	static StaticMeshUPtr CreateSphere
	(
		uint32 latiSegmentCount = 16,
		uint32 longiSegmentCount = 32
	);
};