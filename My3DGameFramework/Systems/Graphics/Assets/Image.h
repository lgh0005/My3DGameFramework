#pragma once
#include "Sources/Asset.h"

struct ktxTexture2;

namespace MGF3D
{
	MGF_CLASS_PTR(Resource)

	MGF_CLASS_PTR(Image)
	class Image : public Asset
	{
		MGF_DISABLE_COPY(Image)
		using Super = Asset;

	public:
		virtual ~Image() override;
		static ImagePtr Create(const String& path);

	/*========================//
	//       Image Type       //
	//========================*/
	public:
		static int16 s_typeIndex;
		virtual const MGFType* GetType() const;

	public:
		virtual bool Load() override;

	private:
		Image(const String& path);
		ResourcePtr CreateTextureResource(ktxTexture2* kTexture);
	};
}