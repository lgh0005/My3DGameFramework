#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(Resource)

	class ResourceManager
	{
		MGF_DECLARE_SINGLE(ResourceManager)

	private:
		ResourceManager();
		~ResourceManager();

	public:
		void Shutdown();
		void RegisterSync(ResourcePtr resource);

	public:
		// 1. 익명 리소스 생성 (캐싱 없음, 즉시 반환)
		template<typename T, typename... Args>
		std::shared_ptr<T> Create(Args&&... args);

		// 2. 익명 리소스 즉시 동기화 생성 (큐 우회, 렌더 타겟 전용)
		template<typename T, typename... Args>
		std::shared_ptr<T> CreateImmediate(Args&&... args);

		// 3. 기명 리소스 생성 및 캐싱 (동일한 이름 요청 시 캐시 반환)
		template<typename T, typename... Args>
		std::shared_ptr<T> GetOrCreate(const String& name, Args&&... args);

		// 4. 기명 리소스 단순 검색
		template<typename T>
		std::shared_ptr<T> Get(const String& name);

	private:
		mutable Mutex m_mutex;
		HashMap<StringHash, ResourcePtr> m_namedCache;
	};
}

#include "Managers/ResourceManager.inl"