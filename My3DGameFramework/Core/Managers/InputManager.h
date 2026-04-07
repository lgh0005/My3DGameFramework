#pragma once

struct GLFWwindow;

namespace MGF3D
{
	MGF_CLASS_PTR(MGFInputDevice)

	class InputManager
	{
		MGF_DECLARE_SINGLE(InputManager)

	private:
		InputManager();
		~InputManager();

	public:
		bool Init(GLFWwindow* window);
		void Update(GLFWwindow* window);
		void Shutdown();

	public:
		template<typename T> T* RegisterDevice();
		template<typename T> T* GetDevice();

	private:
		Vector<MGFInputDeviceUPtr> m_devices;
	};
}

#include "Managers/InputManager.inl"
