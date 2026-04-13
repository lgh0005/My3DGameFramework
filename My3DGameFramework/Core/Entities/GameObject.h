#pragma once
#include "Object.h"

namespace MGF3D
{
	class GameObject : public Object
	{
		MGF_DISABLE_COPY(GameObject)
		using Super = Object;

	public:
		GameObject(ObjectIDHash id, StringHash nameHash);
		virtual ~GameObject() override;
		GameObject(GameObject&&) noexcept;
		GameObject& operator=(GameObject&&) noexcept;

	public:
		StringHash GetNameHash() const;
		void SetActive(bool active);
		bool IsActiveSelf() const;

	private:
		StringHash m_nameHash;
	};
}
