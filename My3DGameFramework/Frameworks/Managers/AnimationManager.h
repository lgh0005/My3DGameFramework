#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(AnimationBuffer)

	class AnimationManager
	{
		MGF_DECLARE_SINGLE(AnimationManager)

	private:
		AnimationManager();
		~AnimationManager();

	public:
		bool Init();
		void Update(float dt);
		void Shutdown();

	public:
		AnimationBuffer* GetAnimationBuffer() { return m_boneBuffer.get(); }

	private:
		AnimationBufferUPtr m_boneBuffer;
	};
}