#pragma once
#include "Registries/IRegistry.h"
#include "Hashes/ObjectIDHash.h"
#include "Types/MGFPackedArray.h"
#include "Entities/GameObject.h"

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
		ObjectIDHash AddGameObject(const String& name);
		GameObject* GetGameObject(const String& name);
		void DestroyGameObject(const String& name);

		GameObject* GetGameObject(ObjectIDHash id);
		void DestroyGameObject(ObjectIDHash id);

	private:
		// 프레임 끝에서 생성/삭제 대기열을 처리합니다.
		void FlushPendingAdds();
		void FlushPendingKills();

		// 이전에 구현한 ID 발급 로직
		ObjectIDHash GenerateNewID();
		void ReleaseID(ObjectIDHash id);

	private:
		void DefragmentGameObject();

		// 1. 실제 데이터 저장소 (PackedArray)
		MGFPackedArray<GameObject> m_gameObjects;

		// 2. 대기열 (Pending Queues)
		Vector<GameObject>     m_pendingAdds;
		Vector<ObjectIDHash>   m_pendingKills;
		Vector<ObjectIDHash>   m_dirtyStateQueue;

		// 3. ID 관리 데이터
		uint32          m_nextIndex	{ 1 };
		Vector<uint32>  m_freeIndices;
		Vector<uint16>  m_generations;
		HashMap<StringHash, ObjectIDHash> m_nameLookupTable;
	};
}