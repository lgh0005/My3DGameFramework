#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Program)
CLASS_PTR(Mesh)
CLASS_PTR(Texture)
CLASS_PTR(CubeTexture)
#pragma endregion

class IBLUtils
{
    DECLARE_STATIC_CLASS(IBLUtils)

public:

    // 1. Spherical Map을 큐브맵으로 변환
	static CubeTexturePtr CreateCubemapFromHDR(Texture* hdrTexture, int32 resolution = 2048);

    // 2. IBL Diffuse용 (Irradiance Map) 생성
    // 큐브맵을 넣으면, 흐릿하게 뭉개진 새 큐브맵을 리턴
    static CubeTexturePtr CreateIrradianceMap(CubeTexture* src);

    // 3. IBL Specular용 (Prefiltered Map) 생성
    // 거칠기별로 밉맵이 생성된 큐브맵 리턴
    static CubeTexturePtr CreatePrefilteredMap(CubeTexture* src);

    // 4. IBL Specular용 (BRDF LUT) 생성
    // 2D 텍스처 하나 리턴
    static TexturePtr CreateBRDFLUT();
};