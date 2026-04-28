#include "GraphicsPch.h"
#include "Image.h"
#include "Sources/Resource.h"
#include "Textures/GLTextureHandle.h"
#include "textures/GLTexture2D.h"
#include "textures/GLTexture3D.h"
#include "textures/GLTextureCube.h"
#include "Managers/TypeManager.h"
#include <ktx.h>

namespace MGF3D
{
	Image::Image(const String& path) : Super(path) { }
	Image::~Image() = default;

	ImagePtr Image::Create(const String& path)
	{
		return SharedPtr<Image>(new Image(path));
	}

	/*========================//
	//       Image Type       //
	//========================*/
	int16 Image::s_typeIndex = -1;
	const MGFType* Image::GetType() const
	{
		MGFTypeTree* tree = MGF_TYPE.GetTree("Asset");
		if (tree != nullptr) return tree->GetType(s_typeIndex);
		return nullptr;
	}

	bool Image::Load()
	{
		// 0. 에셋이 로딩 상태임을 표시
		m_state = EAssetState::Loading;

		// 1. 파일 읽기 (I/O)
		ktxTexture2* kTexture = nullptr;
		KTX_error_code result = ktxTexture2_CreateFromNamedFile
		(
			m_path.c_str(),
			KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
			&kTexture
		);
		if (result != KTX_SUCCESS) return false;

		// 2. BasisU 트랜스코딩
		if (ktxTexture2_NeedsTranscoding(kTexture))
			ktxTexture2_TranscodeBasis(kTexture, KTX_TTF_BC7_RGBA, 0);

		// 3. 리소스 생성
		ResourcePtr res = CreateTextureResource(kTexture);
		if (!res) return false;

		// 4. 빈 텍스쳐 생성 및 데이터 전달
		auto handle = MGFTypeCaster::Cast<GLTextureHandle>(res);
		if (!handle) return false;

		// handle->SetKtxTexture(kTexture);
		handle->SetState(EResourceState::Loaded);

		// 5. 만든 리소스들 저장
		AddResource(handle);
		SetState(EAssetState::Loaded);
		return true;
	}

	ResourcePtr Image::CreateTextureResource(ktxTexture2* kTexture)
	{
		if (kTexture->isCubemap)      return GLTextureCube::Create(kTexture);
		if (kTexture->baseDepth > 1)  return GLTexture3D::Create(kTexture);
		return GLTexture2D::Create(kTexture);
	}
}