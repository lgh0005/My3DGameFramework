#pragma once
#include "Pointer/RefCount.h"
#include "Asset/IAssetDescriptor.h"

namespace MGF3D
{
	enum class AssetState : uint8
	{
		Unloaded = 0,
		Pending,        // 로딩 큐 대기 (AssetManager::m_assets에 등록됨)
		Loading,        // 워커 스레드: OnLoad 실행 중
		WaitingCommit,  // 메인 스레드: OnCommit 대기 중 (Swap 큐에 들어감)
		Ready,          // 사용 가능 (모든 로딩 및 확정 완료)
		Failed          // 로딩 또는 확정 실패
	};

	MGF_CLASS_PTR(MGFType)

	MGF_CLASS_PTR(Asset)
	class Asset : public RefCount
	{
	public:
		Asset(const IAssetDescriptor& desc);
		virtual ~Asset() override;

	public:
		// [Worker Thread] 파일을 읽고 엔진 데이터 구조로 변환
		virtual bool OnLoad() = 0;

		// [Main Thread] 데이터 확정 및 자식 리소스 생성
		virtual bool OnCommit() = 0;

		// [Any Thread] 로딩용 임시 메모리 정리
		virtual void OnRelease() = 0;

	public:
		bool IsReady() const { return GetState() == AssetState::Ready; }
		AssetState GetState() const { return m_state.load(std::memory_order_relaxed); }
		void SetState(AssetState state) { m_state.store(state, std::memory_order_relaxed); }

	public:
		const Ptr<MGFType> GetType() const { return m_type; }
		const MGFName& GetName() const { return m_name; }
		const MGFPath& GetPath() const { return m_path; }

	protected:
		Atomic<AssetState> m_state{ AssetState::Unloaded };
		const Ptr<MGFType> m_type;
		MGFName			   m_name;
		MGFPath            m_path;
	};
}