#pragma once

namespace MGF3D
{
	enum class EResourceState : uint8
	{
		Empty,      // 데이터 없음
		Loading,    // CPU 스레드에서 파일 파싱 및 연산 중
		Loaded,     // CPU 데이터 준비 완료 (파싱 완료)
		Syncing,    // GPU 스레드에서 VRAM 업로드 및 리소스 생성 중
		Ready,      // GPU 리소스 생성 완료 (사용 가능)
		Failed      // 생성 실패
	};

	class Resource
	{
		MGF_DISABLE_COPY(Resource)

	public:
		Resource();
		virtual ~Resource();

	/*==============================//
	//       Resource Type          //
	//==============================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		// GPU 컨텍스트 스레드에서 호출되어 실제 GPU 핸들(VAO, TextureID 등)을 생성함
		virtual bool OnSyncCreate() = 0;

	/*==================================//
	//         State Management         //
	//==================================*/
	public:
		EResourceState GetState() const { return m_state; }
		void SetState(EResourceState state) { m_state = state; }

	protected:
		EResourceState m_state{ EResourceState::Empty };
	};
}