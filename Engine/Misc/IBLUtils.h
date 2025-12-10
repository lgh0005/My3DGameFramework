#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Mesh)
#pragma endregion

class IBLUtils
{
public:
	static CubeTexturePtr CreateCubemapFromHDR(Texture* hdrTexture, int32 resolution = 2048);

    // TODO
    // 2. Diffuse¿ë (Irradiance Map) »ý¼º
    // Å¥ºê¸ÊÀ» ³ÖÀ¸¸é, Èå¸´ÇÏ°Ô ¹¶°³Áø »õ Å¥ºê¸ÊÀ» ¸®ÅÏ
    // static CubeTexturePtr CreateIrradianceMap(CubeTexture* source);

    // 3. Specular¿ë (Prefiltered Map) »ý¼º
    // °ÅÄ¥±âº°·Î ¹Ó¸ÊÀÌ »ý¼ºµÈ Å¥ºê¸Ê ¸®ÅÏ
    // static CubeTexturePtr CreatePrefilteredMap(CubeTexture* source);

    // 4. Specular¿ë (BRDF LUT) »ý¼º
    // 2D ÅØ½ºÃ³ ÇÏ³ª ¸®ÅÏ
    // static TexturePtr CreateBRDFLUT();
};