#pragma once
#include "Pointer/RefCount.h"
#include "Resource/IResourceDescriptor.h"

namespace MGF3D
{
	// 리소스 로딩 상태 (멀티스레드 동기화용)
	enum class ResourceState : uint8
	{
		Unloaded = 0,
		Pending,        // 로딩 큐 대기
		Loading,        // 워커 스레드: OnLoad 실행 중
		WaitingCommit,  // 메인 스레드: OnCommit 대기 중
		Ready,          // 사용 가능
		Failed          // 로딩 실패
	};

	MGF_CLASS_PTR(MGFType)

	MGF_CLASS_PTR(Resource)
	class Resource : public RefCount
	{
	public:
		Resource(const IResourceDescriptor& desc);
		virtual ~Resource() override;

	public:

		// 1. [Worker Thread] 순수 CPU 작업 (I/O, 파싱)
		virtual bool OnLoad() = 0;

		// 2. [Main Thread] API 자원 확정 (GPU 업로드, 전용 등록)
		virtual bool OnCommit() = 0;

		// 3. [Any Thread] 로딩용 임시 데이터 정리
		virtual void OnRelease() = 0;

	public:
		bool IsReady() const { return GetState() == ResourceState::Ready; }
		ResourceState GetState() const { return m_state.load(std::memory_order_acquire); }
		void SetState(ResourceState state) { m_state.store(state, std::memory_order_release); }

	public:
		const Ptr<MGFType> GetType() const { return m_type; }
		const MGFName& GetName() const { return m_name; }

	protected:
		Atomic<ResourceState> m_state { ResourceState::Unloaded };
		const Ptr<MGFType> m_type;
		MGFName m_name;
	};
}