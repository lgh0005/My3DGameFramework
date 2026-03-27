#pragma once
#include "Entities/Entity/Waitables/IWaitable.h"
#include "Asset/IAssetDescriptor.h"

namespace MGF3D
{
	MGF_CLASS_PTR(MGFType)

	MGF_CLASS_PTR(Asset)
	class Asset : public IWaitable
	{
		using Super = IWaitable;

	public:
		Asset(const IAssetDescriptor& desc);
		virtual ~Asset() override;

	public:
		const MGFPath& GetPath() const { return m_path; }

	protected:
		MGFPath            m_path;
	};
}