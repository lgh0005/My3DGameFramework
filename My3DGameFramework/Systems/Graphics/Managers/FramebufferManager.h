#pragma once

namespace MGF3D
{
	class FramebufferManager
	{
		MGF_DECLARE_SINGLE(FramebufferManager)

	private:
		FramebufferManager();
		~FramebufferManager();

	public:
		bool Init();
		void Shutdown();

	private:


	};
}