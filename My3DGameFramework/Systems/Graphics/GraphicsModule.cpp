#include "GraphicsPch.h"
#include "GraphicsModule.h"
#include "Managers/TypeManager.h"
#include "Identities/MGFTypeTree.h"

#include "Textures/GLTextureHandle.h"
#include "Textures/GLTexture2D.h"
#include "Textures/GLTexture3D.h"
#include "Textures/GLTextureCube.h"

#include "Assets/Image.h"
#include "Assets/Shader.h"

namespace MGF3D
{
	void GraphicsModule::OnRegisterTypes()
	{
		// 1. Texture 타입 베이킹
		MGFTypeTree* resourceTree = MGF_TYPE.GetTree("Resource");
		GLTextureHandle::s_typeIndex = resourceTree->Register("TextureHandle", "Resource");
		GLTexture2D::s_typeIndex = resourceTree->Register("Texture2D", "TextureHandle");
		GLTexture3D::s_typeIndex = resourceTree->Register("Texture3D", "TextureHandle");
		GLTextureCube::s_typeIndex = resourceTree->Register("TextureCube", "TextureHandle");

		// 2. Asset 타입 베이킹
		MGFTypeTree* assetTree = MGF_TYPE.GetTree("Asset");
		Image::s_typeIndex = assetTree->Register("Image", "Asset");
		Shader::s_typeIndex = assetTree->Register("Shader", "Asset");
	}

	bool GraphicsModule::OnInit()
	{
		return true;
	}

	bool GraphicsModule::OnShutdown()
	{
		return true;
	}
}