#pragma once
#include "Identities/MGFType.h"
#include "Identities/MGFTypeTree.h"

namespace MGF3D
{
	template<typename To, typename From>
	inline To MGFTypeCaster::Cast(From* obj)
	{
		// 1. To 타입이 포인터인지 컴파일 타임 검증
		MGF_STATIC_ASSERT
		(
			std::is_pointer_v<To>,
			"MGFTypeCaster::Cast - 'To' must be a pointer type."
		);

		// 2. 원본 객체 nullptr 방어
		if (!obj) return nullptr;

		using TargetType = std::remove_pointer_t<To>;
		const MGFType* runtimeType = obj->GetType();

		// 3. 런타임 타입이 없거나, 아직 트리에 Bake되지 않아 ownerTree를 모르면 실패
		if (!runtimeType || !runtimeType->ownerTree) return nullptr;

		// 4. O(1) 상속 족보 검증
		// 트리의 IsA 함수에 현재 객체의 인덱스와, 변환하고자 하는 타겟의 정적 인덱스를 넘겨 묻습니다.
		if (runtimeType->ownerTree->IsA(runtimeType->selfIndex, TargetType::s_typeIndex))
			return static_cast<To>(obj); // 안전 확인 완료, 제일 빠른 캐스팅 수행

		return nullptr;
	}

	template<typename To, typename From>
	inline std::shared_ptr<To> MGFTypeCaster::Cast(const std::shared_ptr<From>& obj)
	{
		// 1. 원본 nullptr 방어
		if (!obj) return nullptr;

		// 2. 런타임 타입 트리 검증 (RTTI 없이 인덱스로 확인)
		const MGFType* runtimeType = obj->GetType();
		if (!runtimeType || !runtimeType->ownerTree) return nullptr;

		// 3. O(1) 계층 구조 확인
		// TargetType::s_typeIndex를 사용하여 부모-자식 관계인지 묻습니다.
		if (runtimeType->ownerTree->IsA(runtimeType->selfIndex, To::s_typeIndex))
			return std::static_pointer_cast<To>(obj);

		return nullptr;
	}

	template<typename To, typename From>
	inline To MGFTypeCaster::ExactCast(From* obj)
	{
		// 1. To 타입이 포인터인지 컴파일 타임 검증
		MGF_STATIC_ASSERT
		(
			std::is_pointer_v<To>,
			"MGFTypeCaster::ExactCast - 'To' must be a pointer type."
		);

		// 2. 원본 객체 nullptr 방어
		if (!obj) return nullptr;

		using TargetType = std::remove_pointer_t<To>;
		const MGFType* runtimeType = obj->GetType();

		// 3. 트리를 거칠 필요도 없이, 자기 자신의 인덱스와 목적지 인덱스가 정확히 같은지만 비교
		if (runtimeType && runtimeType->selfIndex == TargetType::s_typeIndex)
			return static_cast<To>(obj);

		return nullptr;
	}
}