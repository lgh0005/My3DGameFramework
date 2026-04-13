#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(Object)
	class Object
	{
		MGF_DISABLE_COPY(Object)

	public:
		Object(ObjectIDHash id);
		virtual ~Object();
		Object(Object&&) noexcept;
		Object& operator=(Object&&) noexcept;

	public:
		ObjectIDHash GetID() const;
		EObjectLifecycle GetState() const;
		void SetState(EObjectLifecycle newState);

	protected:
		MGFObjectInfo m_info;
	};
}