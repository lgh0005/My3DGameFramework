#pragma once
#include "Pointer/RefCount.h"

namespace MGF3D
{
	enum class WaitableObjectState : uint8
	{
		Unloaded = 0,
		Pending,          // 로딩 큐 대기
		Loading,          // 워커 스레드: OnLoad 실행 중
		WaitingCommit,    // 메인 스레드: OnCommit 대기 중 (Swap 큐에 들어감)
		Ready,            // 사용 가능 (모든 로딩 및 확정 완료)
		Failed            // 로딩 또는 확정 실패
	};

	MGF_CLASS_PTR(IWaitable)
	class IWaitable : public RefCount
	{
	public:
		IWaitable(Ptr<MGFType> type, const MGFName& name);
		virtual ~IWaitable() override;

	public:

		// [Worker Thread] 파일을 읽고 엔진 데이터 구조로 변환
		virtual bool OnLoad() = 0;

		// [Main Thread] 데이터 확정 및 자식 리소스 생성
		virtual bool OnCommit() = 0;

		// [Any Thread] 로딩용 임시 메모리 정리
		virtual void OnRelease() = 0;

	public:
		bool IsReady() const;
		const MGFName& GetName() const;
		WaitableObjectState GetState() const;
		void SetState(WaitableObjectState state);

	protected:
		Atomic<WaitableObjectState> m_state { WaitableObjectState::Unloaded };
		const Ptr<MGFType> m_type;
		MGFName m_name;
	};
}