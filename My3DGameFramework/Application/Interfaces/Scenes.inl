#pragma once

namespace MGF3D
{
	template<typename T>
	inline void Scenes::Register(StringView name)
	{
		MGF_SCENE.RegisterScene<T>(name);
	}

	template<typename T>
	inline void MGF3D::Scenes::Load()
	{
		MGF_SCENE.LoadScene<T>();
	}
}