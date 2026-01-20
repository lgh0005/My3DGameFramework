#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
CLASS_PTR(CubeTexture)
CLASS_PTR(Image)
#pragma endregion

class TextureUtils
{
	DECLARE_STATIC_CLASS(TextureUtils)

/*===========================//
//   texture loading utils   //
//===========================*/
public:
	static TextureUPtr LoadTexture(const std::string& path);
	static TextureUPtr LoadTextureFromImage(const Image* image);
	static TextureUPtr LoadTextureFromHDR(const Image* image);
	static TextureUPtr LoadTextureFromKtx(const std::string& path);

/*================================//
//   cube texture loading utils   //
//================================*/
public:
	static CubeTextureUPtr LoadCubeMapFromImages(const std::vector<Image*> images);
	static CubeTextureUPtr LoadCubeMapFromKtx(const std::string& path);

/*===========================//
//   default texture utils   //
//===========================*/
public:
	static TexturePtr GetWhiteTexture();
	static TexturePtr GetGrayTexture();
	static TexturePtr GetBlackTexture();
	static TexturePtr GetBlueTexture();

/*======================//
//   internal helpers   //
//======================*/
private:
	static TexturePtr Create4x4Texture(const std::vector<uint8>& colorData);
	static TextureUPtr LoadKtxInternal(const std::string& ktxFilePath);
	static CubeTextureUPtr LoadKtxCubeInternal(const std::string& ktxFilePath);
	static void GetFormatsFromImage(const Image* image, GLenum& outInternal, GLenum& outFormat, GLenum& outType);

	static TexturePtr s_whiteTex;
	static TexturePtr s_grayTex;
	static TexturePtr s_blackTex;
	static TexturePtr s_blueTex;
};