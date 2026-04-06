#pragma once
#include "Transform.h"

namespace MGF3D
{
	MGF_CLASS_PTR(TransformStorage)
	class TransformStorage
	{
	public:
		TransformStorage();
		~TransformStorage();

	public:
		usize CreateTransform
		(
			const Position& pos, 
			const Rotation& rot, 
			const Scale& scale
		);
		void SetParent(usize childIndex, usize parentIndex);
		void UpdateWorldMatrices();

	private:
		void UpdateIndexReferences(usize oldIndex, usize newIndex);
		void SortTopologically();

	private:
		SVector<Transform> m_transforms;
		usize m_rootCount{ 0 };
		bool m_needsTopologicalSort{ false };
	};
}