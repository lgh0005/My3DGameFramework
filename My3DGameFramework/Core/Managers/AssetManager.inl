#pragma once

namespace MGF3D
{
    template<typename T>
    SharedPtr<T> AssetManager::LoadAsync(const MGFPath& path, const MGFName& name)
    {
        //// 1. 캐시 키 결정 (이름이 없으면 경로 해시 사용)
        //StringHash key = name.IsEmpty() ? path.GetStringHash() : name.GetStringHash();

        //// 2. 이미 로드된 에셋인지 확인
        //auto it = m_assets.Find(key);
        //if (it) return StaticPointerCast<T>(*it);

        //// 3. 새로 로드해야 함
        //// T::StaticType()을 사용하여 타입 정보를 넘깁니다.
        //auto desc = MakeUnique<IAssetDescriptor>(T::StaticType(), name, path);
        //AssetPtr newAsset = InternalLoadAsync(std::move(desc));
        //if (newAsset)
        //{
        //    m_assets.Insert(key, newAsset);
        //    return StaticPointerCast<T>(newAsset);
        //}

        return nullptr;
    }
}