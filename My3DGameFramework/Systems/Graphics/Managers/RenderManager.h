#pragma once

namespace MGF3D
{
	class RenderManager
	{
		MGF_DECLARE_SINGLE(RenderManager)

	private:
		RenderManager();
		~RenderManager();

	public:
		bool Init();
		void Shutdown();

	private:
	};
}