#pragma once
#include "Managers/ThreadManager.h"
#include "Managers/PathManager.h"
#include "Sources/Asset.h"

namespace MGF3D
{
	template<typename T, typename... Args>
	std::shared_ptr<T> AssetManager::LoadAssetAsync(const String& virtualPath, Args&&... args)
	{
		// 0. T는 Asset을 상속받아야 함.
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<Asset, T>,
			"T must be derived from Asset."
		);

		// 1. 가상 경로를 물리 경로로 변환
		String resolvedPath = MGF_PATH.ResolvePath(virtualPath);

		// 2. 물리 경로 기반의 정수 해시 계산
		PathHash pathHash(resolvedPath);

		// 3. 캐시 검사 및 안전한 타입 캐스팅
		{
			LockScope lock(m_cacheMutex);
			auto it = m_assetCache.find(pathHash);
			if (it != m_assetCache.end())
				return MGFTypeCaster::Cast<T>(it->second);
		}

		// 4. 캐시에 없는 경우 새로 생성
		auto newAsset = T::Create(resolvedPath, std::forward<Args>(args)...);
		if (!newAsset) return nullptr;
		newAsset->SetState(EAssetState::Loading);
		{
			LockScope lock(m_cacheMutex);
			m_assetCache[pathHash] = newAsset;
		}

		// 5. 비동기 로딩 파이프라인 진입
		// AssetManager의 멤버 함수를 호출하기 위해 this와 업캐스팅된 asset을 캡처합니다.
		MGF_THREAD.PushCPUTask
		(
			[this, asset = MGFTypeCaster::Cast<Asset>(newAsset)]()
			{
				ProcessCPULoadTask(asset);
			}
		);

		return newAsset;
	}
}