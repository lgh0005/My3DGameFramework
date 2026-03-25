#pragma once

namespace MGF3D
{
	template<typename T>
	inline SharedPtr<T> AssetManager::Get(const MGFName& assetName)
	{
		StringHash key = assetName.GetStringHash();

		// 2. 타입 검사 및 캐스팅 후 반환
		auto resPtr = m_assets.Find(key);
		if (resPtr)
		{
			//AssetPtr res = *resPtr;
			//if (res->GetType().IsA(T::StaticType()))
			//	return StaticSharedCast<T>(res);
			//MGF_LOG_ERROR
			//(
			//	"ResourceManager: Type mismatch for resource '{}'", 
			//	assetName.CStr()
			//);
		}

		return nullptr;
	}
}