#pragma once

namespace MGF3D
{
	template<typename T>
	inline T* InputManager::RegisterDevice(StringView name)
	{
		MGF_STATIC_ASSERT(std::is_base_of_v<MGFInputDevice, T>,
			"T must be derived from MGFInputDevice.");

		StringHash hash{ name };

		// 이미 존재하는 장치인지 확인
		auto it = m_devices.find(hash);
		if (it != m_devices.end()) return static_cast<T*>(it->second.get());

		// 새 장치 생성 및 등록
		auto device = MakeUnique<T>(String(name));
		T* devicePtr = device.get();
		m_devices[hash] = std::move(device);

		return devicePtr;
	}

	template<typename T>
	inline T* MGF3D::InputManager::GetDevice(StringView name)
	{
		MGF_STATIC_ASSERT(std::is_base_of_v<MGFInputDevice, T>,
			"T must be derived from MGFInputDevice.");

		StringHash hash{ name };
		auto it = m_devices.find(hash);
		if (it != m_devices.end()) return static_cast<T*>(it->second.get());
		return nullptr;
	}
}