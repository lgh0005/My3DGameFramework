#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(GLShaderStorageBuffer)

	MGF_CLASS_PTR(AnimationBuffer)
	class AnimationBuffer
	{
		MGF_DISABLE_COPY(AnimationBuffer)

	public:
		AnimationBuffer();
		~AnimationBuffer();

		bool Init(usize maxBoneCapacity);
		void Clear();

		uint32 PushBones(const Vector<mat4>& boneMatrices);
		void UploadToGPU();
		void Bind(uint32 bindingIndex) const;

		uint32 GetCurrentBoneCount() const { return m_currentBoneCount; }

	private:
		Vector<mat4> m_cpuBuffer;
		GLShaderStorageBufferUPtr m_gpuBuffer{ nullptr };

		uint32 m_currentBoneCount{ 0 };
		uint32 m_maxBoneCapacity{ 0 };
	};
}