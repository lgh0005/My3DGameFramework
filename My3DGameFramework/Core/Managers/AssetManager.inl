#pragma once

namespace MGF3D
{
    template<typename T>
    SharedPtr<T> AssetManager::LoadAsync(const MGFPath& path, const MGFName& name)
    {
        //// 1. 캐시 키 결정
        //StringHash key = name.IsEmpty() ? path.GetStringHash() : name.GetStringHash();

        //// 2. 이미 로드된 에셋인지 확인
        //auto it = m_assets.Find(key);
        //if (it)
        //{
        //    AssetPtr existingAsset = *it;
        //    if (existingAsset->GetType()->IsA(T::StaticType()))
        //    {
        //        return StaticPointerCast<T>(existingAsset);
        //    }
        //    MGF_LOG_ERROR("AssetManager: Asset type mismatch for key '{}'", name.IsEmpty() ? path.GetCStr() : name.CStr());
        //    return nullptr;
        //}

        //// 3. 로더 찾기 (T::StaticType() 활용)
        //const MGFType* targetType = T::StaticType();
        //auto loaderPtr = m_loaders.Find(targetType);
        //if (!loaderPtr)
        //{
        //    MGF_LOG_ERROR("AssetManager: No loader registered for type '{}'", targetType->name.CStr());
        //    return nullptr;
        //}

        //// 4. 에셋 인스턴스(껍데기) 생성
        //auto desc = MakeUnique<IAssetDescriptor>(targetType, name, path);
        //AssetPtr newAsset = nullptr;

        //// 로더를 통해 구체적인 에셋 객체 생성
        //if (!(*loaderPtr)->Load(newAsset, std::move(desc)))
        //{
        //    MGF_LOG_ERROR("AssetManager: Failed to create asset instance for '{}'", path.GetCStr());
        //    return nullptr;
        //}

        //// 5. 비동기 로딩 태스크 발행
        //auto loadTask = MGF_TASK.AcquireTask([newAsset]()
        //    {
        //        MGF_LOG_INFO("AssetManager: Starting async load -> {}", newAsset->GetName().CStr());
        //        if (!newAsset->OnLoad())
        //        {
        //            MGF_LOG_ERROR("AssetManager: OnLoad failed for asset -> {}", newAsset->GetName().CStr());
        //        }
        //    });

        //MGF_TASK.PushTask(std::move(loadTask));

        //// 6. 캐시에 등록 및 결과 반환
        //m_assets.Insert(key, newAsset);
        //return StaticPointerCast<T>(newAsset);

        return nullptr;
    }
}