#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(Resource)

	enum class EAssetState : uint8
	{
		Empty,      // 아무 작업도 시작되지 않음
		Loading,    // CPU에서 파일 읽기/파싱 중
		Loaded,     // 데이터 파싱 완료, 리소스들이 생성됨 (GPU 대기)
		Syncing,    // 3: 자식 리소스들이 GPU 업로드 중
		Ready,      // 모든 내부 리소스가 사용 가능한 상태
		Failed      // 로드 실패
	};

	class Asset
	{
		MGF_DISABLE_COPY(Asset)

	public:
		Asset(const String& path);
		virtual ~Asset();

	/*==============================//
	//       Asset Type             //
	//==============================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		// CPU 바운드 일거리를 비동기로 로드 (파일 I/O)
		virtual bool Load() = 0;

		// 생성한 리소스들을 저장
		void AddResource(const ResourcePtr& resource);
		const Vector<ResourcePtr>& GetResources() const { return m_resources; }

	/*==================================//
	//         State Management         //
	//==================================*/
	public:
		EAssetState GetState() const;
		void SetState(EAssetState state) { m_state = state; }

	protected:
		String m_path;
		EAssetState m_state	{ EAssetState::Empty };
		Vector<ResourcePtr> m_resources;
	};
}