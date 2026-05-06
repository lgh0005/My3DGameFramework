#pragma once

namespace MGF3D
{
	class AnimClip
	{
		MGF_DISABLE_COPY(AnimClip)

	public:
		AnimClip();
		AnimClip
		(
			uint32 frameCount, 
			uint32 boneCount, 
			float frameRate, 
			Vector<mat4>&& matrices
		);
		~AnimClip();
		AnimClip(AnimClip&& other) noexcept;
		AnimClip& operator=(AnimClip&& other) noexcept;

	public:
		bool IsValid() const;
		void Clear();
		float GetDuration() const;
		float GetFrameRate() const { return m_frameRate; }
		const mat4& GetLocalMatrix(uint32 frame, uint32 boneIndex) const;
		uint32 GetFrameCount() const { return m_frameCount; }
		uint32 GetBoneCount() const { return m_boneCount; }

	private:
		Vector<mat4> m_localMatrices;
		uint32 m_frameCount		{ 0 };
		uint32 m_boneCount		{ 0 };
		float m_frameRate		{ 30.0f };
	};
}
