#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(Component)

	class Mechanic
	{
	public:
		Mechanic(Component* owner);
		virtual ~Mechanic();

	public:
		void SetOwner(Component* owner);
		Component* GetOwner() const;

	protected:
		Component* m_owner	{ nullptr };
	};
}