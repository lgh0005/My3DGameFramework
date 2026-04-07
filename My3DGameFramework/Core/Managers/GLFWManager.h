#pragma once

namespace MGF3D
{
	class GLFWManager
	{
		MGF_DECLARE_SINGLE(GLFWManager)

	public:
		GLFWManager();
		~GLFWManager();

	public:
		bool Init();
		void Shutdown();
		void Update();

	private:
		static void ErrorCallback(int32 err, cstr desc);
	};
}