#pragma once
#include "Entities/ECS/Chunk.h"
#include "Entities/ECS/IStorage.h"
// #include "Entities/Entity/Component.h"

// INFO : 너무 어렵게 생각하지 말고
// 기존에 우리가 arrray<ComponentType, vector<T>>로다가 배열을 두는 것이었다면
// 이제는 이걸 조금 더 클래스로 분화했다고 생각하면 된다.
// 보다 컨테이너 내부에 대한 컴포넌트를 보다 정밀하게 제어를 할 수 있고,
// 이제 Chunk라는 거대한 메모리 위에서 캐시 히트율이 높은 방식으로 구현이 가능할
// 것으로 기대한다.

namespace MGF3D
{
	template<typename T>
	class Storage : public IStorage
	{
		// MGF_STATIC_ASSERT(std::is_base_of_v<Component, T>, "Storage<T>: T must inherit from MGF3D::Component!");

	public:
		template<typename... Args> T* Emplace(Args&&... args);

	public:
		virtual void Update() override;
		virtual void FixedUpdate() override;
		virtual void LateUpdate() override;

		virtual void Clear() override;
		virtual void Remove(usize index);
		virtual void Defragment();

	private:
		Chunk<T> m_chunk;
		SVector<Ptr<T>> m_denseArray;
	};
}

#include "Entities/ECS/Storage.inl"