#pragma once

namespace MGF3D
{
	template<typename T>
	inline T* InputManager::RegisterDevice()
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<MGFInputDevice, T>,
			"T must be derived from MGFInputDevice."
		);

		const int16 idx = T::s_typeIndex;
		if (idx == -1) return nullptr;

		// 1. Vector 크기가 부족하면 확장
		if (static_cast<usize>(idx) >= m_devices.size())
			m_devices.resize(idx + 1);

		// 2. 이미 해당 타입의 장치가 등록되어 있는지 확인
		if (m_devices[idx] != nullptr)
			return MGFTypeCaster::Cast<T*>(m_devices[idx].get());

		// 3. 새 장치 생성
		auto device = MakeUnique<T>();
		T* devicePtr = device.get();
		m_devices[idx] = std::move(device);

		MGF_LOG_INFO("InputManager: Registered device at index {0}", idx);
		return devicePtr;
	}

	template<typename T>
	inline T* InputManager::GetDevice()
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<MGFInputDevice, T>,
			"T must be derived from MGFInputDevice."
		);

		const int16 idx = T::s_typeIndex;
		if (!CommonUtils::IsBetween<usize>(idx, 0, m_devices.size()))
			return nullptr;

		return MGFTypeCaster::Cast<T*>(m_devices[idx].get());
	}
}