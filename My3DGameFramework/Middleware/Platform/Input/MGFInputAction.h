#pragma once
#include "Pointer/PoolAlloc.h"

namespace MGF3D
{
	enum class InputEvent : uint8
	{
		Pressed,
		Released,
		MAX
	};

	MGF_CLASS_PTR(MGFInputAction)
	class MGFInputAction : public PoolAlloc
	{
	public:
		using InputCallback = Action<>;
		MGFInputAction(const SString& name);
		virtual ~MGFInputAction() override;

	public:
		void AddKeyBinding(int32 keyCode);
		void AddMouseButtonBinding(int32 button);
		void BindCallback(InputEvent event, InputCallback callback);
		void Invoke(InputEvent event);
		void Release();

	public:
		const SString& GetName() const { return m_name; }
		const SVector<int32>& GetMappedKeys() const { return m_mappedKeys; }
		const SVector<int32>& GetMappedMouseButtons() const { return m_mappedMouseButtons; }

	private:
		SString m_name;
		SVector<int32> m_mappedKeys;
		SVector<int32> m_mappedMouseButtons;
		SVector<InputCallback> m_callbacks[(int32)InputEvent::MAX];
	};
}