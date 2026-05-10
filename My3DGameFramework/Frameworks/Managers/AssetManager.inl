#pragma once
#include "Managers/ThreadManager.h"
#include "Managers/PathManager.h"
#include "Sources/Asset.h"
#include "Sources/Resource.h"

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

	template<typename T>
	std::shared_ptr<T> AssetManager::GetAsset(const String& virtualPath)
	{
		MGF_STATIC_ASSERT
		(
			std::is_base_of_v<Asset, T>, 
			"T must be derived from Asset."
		);

		String resolvedPath = MGF_PATH.ResolvePath(virtualPath);
		PathHash pathHash(resolvedPath);

		std::shared_ptr<T> asset = nullptr;

		// 1. 캐시 검사 및 에셋 포인터 획득
		{
			LockScope lock(m_cacheMutex);
			auto it = m_assetCache.find(pathHash);
			if (it != m_assetCache.end()) asset = MGFTypeCaster::Cast<T>(it->second);
		}

		// 2. 캐시에 에셋이 등록된 적이 없다면 nullptr
		if (!asset) return nullptr;

		// 3. 로딩 중이라면 스레드 대기
		while (asset->GetState() == EAssetState::Loading)
			std::this_thread::yield();

		// 2. 지연 평가 (Lazy Evaluation): 
		// GPU 워커에서 작업 중인(Syncing) 상태일 때 누군가 에셋을 요청하면, 
		// 하위 리소스들이 모두 구워졌는지 확인하고 상태를 업데이트합니다.
		if (asset->GetState() == EAssetState::Syncing)
		{
			bool bAllReady = true;
			for (const auto& res : asset->GetResources())
			{
				if (res->GetState() != EResourceState::Ready)
				{
					bAllReady = false;
					break;
				}
			}

			if (bAllReady)
				asset->SetState(EAssetState::Ready);
		}

		// 3. 실패 상태만 아니라면 에셋 반환 
		// (Syncing 상태여도 렌더러가 내부적으로 리소스 Ready 체크를 수행하므로 껍데기를 줍니다)
		if (asset->GetState() != EAssetState::Failed)
			return MGFTypeCaster::Cast<T>(asset);

		return nullptr;
	}
}