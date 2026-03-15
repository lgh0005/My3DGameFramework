#pragma once

namespace MGF3D
{
	struct Position;
	struct Rotation;
	struct Scale;

	class CoordFrame
	{
	public:
		CoordFrame();

		void SetCoordDirty();

		void Update
		(
			const Position& pos,
			const Rotation& rot,
			const Scale& scale
		);

		const mat4& GetModelMatrix() const;
		const mat4& GetInverseMatrix() const;

		vec3 ToWorldPoint(const vec3& localPoint) const;
		vec3 ToLocalPoint(const vec3& worldPoint) const;
		vec3 ToWorldDir(const vec3& localDir) const;
		vec3 ToLocalDir(const vec3& worldDir) const;

	private:
		mat4 m_modelMatrix{ 1.0f };
		bool m_isCoordDirty{ true };
		
		mutable mat4 m_inverseMatrix{ 1.0f };
		mutable bool m_isInverseDirty{ true };
	};
}