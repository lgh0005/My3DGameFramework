#pragma once

namespace MGF3D
{
	template<typename T>
	inline void GeometryUtils::ComputeTangents(Vector<T>& vertices, const Vector<uint32>& indices)
	{
		if (vertices.empty() || indices.empty()) return;

		Vector<vec3> tangents;
		tangents.resize(vertices.size(), vec3(0.0f));

		for (usize i = 0; i < indices.size(); i += 3)
		{
			int32 i0 = indices[i];
			uint32 i1 = indices[i + 1];
			uint32 i2 = indices[i + 2];

			const auto& v0 = vertices[i0];
			const auto& v1 = vertices[i1];
			const auto& v2 = vertices[i2];

			// Edge(변) 벡터
			vec3 edge1 = v1.position - v0.position;
			vec3 edge2 = v2.position - v0.position;

			// Delta UV 벡터
			vec2 deltaUV1 = v1.texCoord - v0.texCoord;
			vec2 deltaUV2 = v2.texCoord - v0.texCoord;

			// 행렬식(Determinant) 계산
			float det = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			if (det != 0.0f)
			{
				float invDet = 1.0f / det;

				vec3 tangent;
				tangent.x = invDet * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
				tangent.y = invDet * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
				tangent.z = invDet * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

				tangents[i0] += tangent;
				tangents[i1] += tangent;
				tangents[i2] += tangent;
			}
			else
			{
				tangents[i0] += edge1;
				tangents[i1] += edge1;
				tangents[i2] += edge1;
			}
		}

		// 정규화 (Normalize)
		for (usize i = 0; i < vertices.size(); i++)
		{
			if (Math::HasLength(tangents[i])) vertices[i].tangent = normalize(tangents[i]);
			else vertices[i].tangent = vec3(1.0f, 0.0f, 0.0f);
		}
	}
}