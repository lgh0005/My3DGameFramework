#include "FrameworkPch.h"
#include "AnimationBuffer.h"
#include "Buffers/GLShaderStorageBuffer.h"

namespace MGF3D
{
	AnimationBuffer::AnimationBuffer() = default;
	AnimationBuffer::~AnimationBuffer() = default;

	bool AnimationBuffer::Init(usize maxBoneCapacity)
	{
		m_maxBoneCapacity = maxBoneCapacity;
		m_cpuBuffer.resize(maxBoneCapacity);
		m_gpuBuffer = GLShaderStorageBuffer::Create(nullptr, maxBoneCapacity * sizeof(mat4));
		return m_gpuBuffer != nullptr;
	}

	void AnimationBuffer::Clear()
	{
		m_currentBoneCount = 0;
	}

	uint32 AnimationBuffer::PushBones(const Vector<mat4>& boneMatrices)
	{
		uint32 boneCount = (uint32)boneMatrices.size();
		if (boneCount == 0) return 0;

		// 버퍼 용량 초과 방지
		if (m_currentBoneCount + boneCount > m_maxBoneCapacity)
		{
			MGF_LOG_ERROR("AnimationBuffer: Max capacity exceeded!");
			return 0;
		}

		uint32 offset = m_currentBoneCount;

		// CPU 버퍼에 데이터 복사
		std::memcpy
		(
			&m_cpuBuffer[offset],
			boneMatrices.data(),
			boneCount * sizeof(mat4)
		);

		m_currentBoneCount += boneCount;
		return offset;
	}

	void AnimationBuffer::UploadToGPU()
	{
		if (m_currentBoneCount == 0 || m_gpuBuffer == nullptr) return;
		m_gpuBuffer->UpdateData(m_cpuBuffer.data(), 0, m_currentBoneCount * sizeof(mat4));
	}

	void AnimationBuffer::Bind(uint32 bindingIndex) const
	{
		if (m_gpuBuffer) m_gpuBuffer->Bind(bindingIndex);
	}
}