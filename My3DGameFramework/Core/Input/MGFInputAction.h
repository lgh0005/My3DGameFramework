#pragma once

struct GLFWwindow;

namespace MGF3D
{
	enum class InputEvent : uint8
	{
		Pressed,
		Released,
		MAX
	};

	MGF_CLASS_PTR(MGFInputAction)
	class MGFInputAction
	{
	public:
		MGFInputAction(const String& name);
		virtual ~MGFInputAction();

	public:
		virtual bool IsMapped(int32 inputCode) const = 0;
		virtual bool CheckRawState(GLFWwindow* window) const = 0;

	public:
		void BindCallback(InputEvent event, const Action<>& callback);
		void Invoke(InputEvent event);
		void Release();

	public:
		const String& GetName() const { return m_name; }

	protected:
		String m_name;
		Vector<Action<>> m_callbacks[(int32)InputEvent::MAX];
	};
}