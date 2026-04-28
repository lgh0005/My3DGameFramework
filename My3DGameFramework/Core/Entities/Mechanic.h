#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(Component)

	class Mechanic
	{
		MGF_DISABLE_COPY(Mechanic)

	public:
		Mechanic(Component* owner);
		virtual ~Mechanic();
		Mechanic(Mechanic&& other) noexcept;
		Mechanic& operator=(Mechanic&& other) noexcept;

	/*================================//
	//   MGF3D Component Custom Type  //
	//================================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		void SetOwner(Component* owner);
		Component* GetOwner() const;

	protected:
		Component* m_owner	{ nullptr };
	};
}