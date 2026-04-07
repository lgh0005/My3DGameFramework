#pragma once

namespace MGF3D
{
	class MGFTypeCaster
	{
		MGF_DECLARE_UTILITIES(MGFTypeCaster)

	public:

		// 1. 다형성 캐스팅 (상속 관계 허용)
		// 객체의 런타임 타입이 To이거나, To의 자손일 경우 변환합니다.
		template <typename To, typename From> static To Cast(From* obj);

		// 2. 엄격한 캐스팅 (정확히 일치하는 타입만 허용)
		// 객체의 런타임 타입이 To와 100% 동일할 때만 변환합니다.
		template <typename To, typename From> static To ExactCast(From* obj);
	};
}

#include "Identities/MGFTypeCaster.inl"