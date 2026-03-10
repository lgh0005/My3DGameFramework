#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace MGF3D
{
	class GLFWManager
	{
		MGF_DECLARE_SINGLE(GLFWManager)

	private:
		GLFWManager();
		~GLFWManager();

	public:
		bool Init();
		void Shutdown();
		void Update();

	public:
		double GetTime();

	private:
		static void ErrorCallback(int32 err, cstr desc);
	};
}