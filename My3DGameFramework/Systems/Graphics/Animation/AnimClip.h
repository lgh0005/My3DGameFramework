#pragma once

namespace MGF3D
{
	class AnimClip
	{
		MGF_DISABLE_COPY(AnimClip)

	public:
		AnimClip();
		~AnimClip();
		AnimClip(AnimClip&& other) noexcept;
		AnimClip& operator=(AnimClip&& other) noexcept;

	public:
		bool IsValid() const;
		void Clear();

	public:
		SVector<mat4> localMatrices;
		uint32 frameCount = 0;
		uint32 boneCount = 0;
		float frameRate = 30.0f;
	};
}
