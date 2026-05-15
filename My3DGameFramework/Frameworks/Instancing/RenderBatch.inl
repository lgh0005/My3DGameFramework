#pragma once
#include "Buffers/GLShaderStorageBuffer.h"
#include "Graphics/Programs/GraphicsProgram.h"

namespace MGF3D
{
	template<typename T>
	void RenderBatch<T>::Draw
	(
		uint32 bindingSlot,
		GLShaderStorageBuffer* instanceBuffer,
		GraphicsProgram* overrideProgram
	)
	{
		if (m_instances.empty() || !m_mesh || !m_material) return;
		MGF_ASSERT(m_mesh->GetState() == EResourceState::Ready, "Mesh must be 'ready' state here.");
		MGF_ASSERT(m_material->GetState() == EResourceState::Ready, "Material must be 'ready' state here.");

		// 1. 머티리얼 또는 오버라이드 셰이더 바인딩
		if (overrideProgram != nullptr) overrideProgram->Use();
		else m_material->Bind();

		// 2. 인스턴스 데이터 전송 및 바인딩
		usize requiredSize = m_instances.size() * sizeof(T);

		// 데이터 업로드
		instanceBuffer->UpdateData(m_instances.data(), 0, requiredSize);
		instanceBuffer->Bind(bindingSlot);

		// 3. 메쉬 바인딩 및 드로우 콜
		m_mesh->Bind();
		glDrawElementsInstanced
		(
			GL_TRIANGLES,									// 모드 (보통 삼각형)
			static_cast<GLsizei>(m_mesh->GetIndexCount()),  // 인덱스 개수
			GL_UNSIGNED_INT,								// 인덱스 타입
			nullptr,										// 이정표 (EBO 사용 시 nullptr)
			static_cast<GLsizei>(m_instances.size())		// 인스턴스 개수 (중요!)
		);
	}

	template<typename T>
	void RenderBatch<T>::SetMesh(Mesh* mesh)
	{
		m_mesh = mesh;
	}

	template<typename T>
	void RenderBatch<T>::SetMaterial(Material* material)
	{
		m_material = material;
	}

	template<typename T>
	void RenderBatch<T>::AddInstance(const T& data)
	{
		m_instances.push_back(data);
	}

	template<typename T>
	void RenderBatch<T>::Clear()
	{
		m_instances.clear();
	}
}