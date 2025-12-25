#version 460 core

// TODO : 이후에 spv로 미리 컴파일 시키려면 보다 엄격히 적어야 할 필요가 있다.

// TODO : 인풋과 아웃풋은 슬슬 하나의 묶음으로 처리
// 할 필요가 있다. vs_in, vs_out, fs_in, fs_out 등과 같이.

layout (location = 0) out vec4 gPosition;      // Attachment 0
layout (location = 1) out vec4 gNormal;        // Attachment 1
layout (location = 2) out vec4 gAlbedoSpec;    // Attachment 2 (RGB: Albedo, A: Specular)
layout (location = 3) out vec4 gEmission;      // Attachment 3

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;

layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

struct Material 
{
    // Textures
    sampler2D diffuse;
    sampler2D specular; // (Legacy) PBR에서는 잘 안 씀
    sampler2D emission;
    sampler2D normal;
    sampler2D height;

    sampler2D orm;      // R:AO, G:Rough, B:Metal
    sampler2D ao;       
    sampler2D metallic;
    sampler2D roughness;

    // Factors
    float shininess;        // (Legacy)
    float emissionStrength;
    float heightScale;

    vec4  albedoFactor;
    
    // [New] PBR Factors
    float roughnessFactor;
    float metallicFactor;
    vec3  emissiveFactor;

    bool  hasORM;
};
uniform Material material;

// Parallax Mapping 함수
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(viewDir.z));  
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    
    vec2 P = viewDir.xy / viewDir.z * material.heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    vec2 currentTexCoords = texCoords;

    // [수정 1] texture -> textureLod(..., 0.0)
    // Height 맵 읽을 때는 밉맵 필요 없음. 강제로 0번 레벨 사용.
    float currentDepthMapValue = 1.0 - textureLod(material.height, currentTexCoords, 0.0).r;
      
    while (currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        // [수정 2] textureLod
        currentDepthMapValue = 1.0 - textureLod(material.height, currentTexCoords, 0.0).r;  
        currentLayerDepth += layerDepth;  
    }
    
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    
    // [수정 3] textureLod
    float beforeDepth = (1.0 - textureLod(material.height, prevTexCoords, 0.0).r) - (currentLayerDepth - layerDepth);
 
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main()
{
// [핵심 1] 패럴랙스 계산 전, 원래 UV의 기울기(Gradient) 저장
    vec2 ddx = dFdx(TexCoords);
    vec2 ddy = dFdy(TexCoords);

    vec2 texCoord = TexCoords;

    // 1. TBN 재정렬 및 패럴랙스 매핑
    vec3 T = normalize(TBN[0]);
    vec3 B = normalize(TBN[1]);
    vec3 N = normalize(TBN[2]);
    mat3 orthoTBN = mat3(T, B, N);
    
    // HeightScale이 유의미할 때만 패럴랙스 수행
    if (material.heightScale > 0.001)
    {
        vec3 viewDir = normalize(viewPos - FragPos);
        mat3 worldToTangent = transpose(orthoTBN);
        vec3 viewDirTangent = normalize(worldToTangent * viewDir);
        
        // 위에서 고친 함수 호출
        texCoord = ParallaxMapping(TexCoords, viewDirTangent);
        
        // 만약 UV가 범위를 벗어나면 버리는 로직 (선택사항)
        if (texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0) 
            discard;
    }

    // 기본값 설정 (Factor 적용)
    float ao = 1.0;
    float roughness = material.roughnessFactor;
    float metallic  = material.metallicFactor;

    if (material.hasORM)
    {
        // [Optimized Path] ORM 텍스처 한 번만 샘플링
        // R: Occlusion, G: Roughness, B: Metallic
        // textureGrad를 사용하여 Parallax로 인한 UV 왜곡에도 선명하게 샘플링
        vec3 ormSample = textureGrad(material.orm, texCoord, ddx, ddy).rgb;
        ao = ormSample.r;
        roughness *= ormSample.g; // Factor * Texture
        metallic  *= ormSample.b; // Factor * Texture
    }
    else
    {
        // [Legacy Path] 개별 텍스처 샘플링
        ao = textureGrad(material.ao, texCoord, ddx, ddy).r;
        roughness *= textureGrad(material.roughness, texCoord, ddx, ddy).r;
        metallic  *= textureGrad(material.metallic, texCoord, ddx, ddy).r;
    }

    // G-Buffer 채우기 (URP Layout)
    gPosition.rgb = FragPos;
    gPosition.a = ao;

    // [Attachment 1] Normal(RGB) + Roughness(A)
    vec3 normalMapValue = textureGrad(material.normal, texCoord, ddx, ddy).rgb;
    normalMapValue = normalize(normalMapValue * 2.0 - 1.0);
    gNormal.rgb = normalize(orthoTBN * normalMapValue);
    gNormal.a = roughness;

    // [Attachment 2] Albedo(RGB) + Metallic(A)
    vec4 albedoSample = textureGrad(material.diffuse, texCoord, ddx, ddy);
    vec3 finalAlbedo = albedoSample.rgb * material.albedoFactor.rgb;

    // Alpha Masking
    if (albedoSample.a * material.albedoFactor.a < 0.1) discard;

    gAlbedoSpec.rgb = finalAlbedo;
    gAlbedoSpec.a = metallic;

    // [Attachment 3] Emission
    // Emission Texture * Strength + Emission Factor
    vec3 emissionTex = textureGrad(material.emission, texCoord, ddx, ddy).rgb;
    vec3 finalEmission = (emissionTex * material.emissionStrength) + material.emissiveFactor;
    gEmission = vec4(finalEmission, 1.0);
}