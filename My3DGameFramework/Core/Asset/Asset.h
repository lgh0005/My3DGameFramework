#pragma once
#include "Pointer/RefCount.h"
#include "Asset/IAssetDescriptor.h"

namespace MGF3D
{
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

		// [Any Thread] 메모리에서 해제될 때 정리 로직
		virtual void OnUnload() = 0;

	public:
		const Ptr<MGFType> GetType() const { return m_type; }
		const MGFName& GetName() const { return m_name; }
		const MGFPath& GetPath() const { return m_path; }

	protected:
		const Ptr<MGFType> m_type;
		MGFName			   m_name;
		MGFPath            m_path;
	};
}