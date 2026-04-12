#pragma once
#include "Registries/IRegistry.h"
#include "Hashes/ObjectIDHash.h"
#include "Types/MGFPackedArray.h"
#include "Identities/MGFObjectInfo.h"

namespace MGF3D
{
	class GameObjectRegistry : public IRegistry
	{
	public:
		GameObjectRegistry();
		virtual ~GameObjectRegistry();

	public:
		virtual void Init() override;
		virtual void Update() override;
		virtual void Shutdown() override;
		virtual void Clear() override;

	public:
		ObjectIDHash AddGameObject(const GameObjectInfo& info);
		void DestroyGameObject(ObjectIDHash id);

	private:
		// 프레임 끝에서 생성/삭제 대기열을 처리합니다.
		void FlushPendingAdds();
		void FlushPendingKills();

		// 이전에 구현한 ID 발급 로직
		ObjectIDHash GenerateNewId();
		void ReleaseId(ObjectIDHash id);

	private:
		// 1. 실제 데이터 저장소 (PackedArray)
		// GameObject 클래스는 아직 정의 전이라 가정합니다.
		// MGFPackedArray<GameObject> m_gameObjects;

		// 2. 대기열 (Pending Queues)
		Vector<MGFObjectInfo> m_pendingAdds;
		Vector<ObjectIDHash>   m_pendingKills;

		// 3. ID 관리 데이터
		uint32          m_nextIndex	{ 1 };
		Vector<uint32>  m_freeIndices;
		Vector<uint16>  m_generations;
	};
}