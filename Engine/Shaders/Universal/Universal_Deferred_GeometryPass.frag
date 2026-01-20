#version 460 core

layout (location = 0) out vec4 gPosition;      // Attachment 0
layout (location = 1) out vec4 gNormal;        // Attachment 1
layout (location = 2) out vec4 gAlbedoSpec;    // Attachment 2 (RGB: Albedo, A: Metallic)
layout (location = 3) out vec4 gEmission;      // Attachment 3
layout (location = 4) out vec2 gVelocity;      // Attachment 4: 속도 벡터

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;

in vec4 vCurrClipPos;
in vec4 vPrevClipPos;

layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    sampler2D normal;
    sampler2D height; 

    sampler2D orm;      // R:AO, G:Rough, B:Metal
    sampler2D ao;        
    sampler2D metallic;
    sampler2D roughness;

    float shininess;        
    float emissionStrength;
    float heightScale;

    vec4  albedoFactor;
    
    float roughnessFactor;
    float metallicFactor;
    vec3  emissiveFactor;

    bool  hasORM;
};
uniform Material material;

// [Safety] 안전한 정규화 함수
vec3 SafeNormalize(vec3 v)
{
    if (dot(v, v) < 1e-6) return vec3(0, 0, 1);
    return normalize(v);
}

bool IsNaN(float val) { return (val != val); }
bool IsNaN2(vec2 v) { return IsNaN(v.x) || IsNaN(v.y); }

// [Parallax] Parallax Occlusion Mapping 함수
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    // ★ [핵심 안전장치 1] 시선이 표면과 거의 수평일 때(z < 0.0) 계산 폭발 방지
    // 이 부분이 없으면 가장자리에서 검은 점이 생기거나 텍스처가 깨짐
    if (viewDir.z < 1e-3) return texCoords;

    // 레이어 개수 조절 (가파른 각도일수록 더 정밀하게)
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(viewDir.z));  
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    
    // P = 오프셋 벡터 (z값으로 나누기 때문에 z가 0이면 위험함 -> 위에서 방어함)
    vec2 P = viewDir.xy / viewDir.z * material.heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    vec2 currentTexCoords = texCoords;

    // ★ [핵심 안전장치 2] Loop 안에서는 textureLod 사용
    // 일반 texture()는 Loop 안에서 미분값을 잃어버려 뭉개질 수 있음
    float currentDepthMapValue = 1.0 - textureLod(material.height, currentTexCoords, 0.0).r;
      
    while (currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = 1.0 - textureLod(material.height, currentTexCoords, 0.0).r;  
        currentLayerDepth += layerDepth;  
    }
    
    // Parallax Occlusion Interpolation (더 부드러운 결과)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (1.0 - textureLod(material.height, prevTexCoords, 0.0).r) - (currentLayerDepth - layerDepth);
 
    float denominator = afterDepth - beforeDepth;
    float weight = 0.0;
    if (abs(denominator) > 1e-5)
    {
        weight = afterDepth / denominator;
    }
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main()
{
    // ★ [핵심 안전장치 3] 원본 UV의 미분값(기울기) 미리 저장
    // 패럴랙스로 UV가 변해도, 밉맵 레벨은 이 값을 기준으로 계산해야 선명함 (Aliasing 방지)
    vec2 ddx = dFdx(TexCoords);
    vec2 ddy = dFdy(TexCoords);

    // 1. TBN 재정렬 (안전 장치 포함)
    vec3 T = SafeNormalize(TBN[0]);
    vec3 B = SafeNormalize(TBN[1]);
    vec3 N = SafeNormalize(TBN[2]);
    mat3 orthoTBN = mat3(T, B, N);

    vec2 texCoord = TexCoords;

    // 2. Parallax Mapping 수행
    if (material.heightScale > 0.001)
    {
        // View Direction을 Tangent Space로 변환
        vec3 viewDir = SafeNormalize(viewPos - FragPos);
        mat3 worldToTangent = transpose(orthoTBN);
        vec3 viewDirTangent = SafeNormalize(worldToTangent * viewDir);
        
        // 패럴랙스 함수 호출
        texCoord = ParallaxMapping(TexCoords, viewDirTangent);
        if (IsNaN2(texCoord)) 
        {
            discard; // 계산 터지면 그냥 버림
        }

        // UV가 범위를 벗어났을 때 처리 (선택: discard or clamp or wrap)
        if(texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0)
            discard;
    }
    
    // 3. PBR 파라미터 샘플링 (★ textureGrad 사용 필수)
    float ao = 1.0;
    float roughness = material.roughnessFactor;
    float metallic  = material.metallicFactor;

    if (material.hasORM)
    {
        // texture 대신 textureGrad를 써야 패럴랙스 적용 후에도 텍스처가 선명함
        vec3 ormSample = textureGrad(material.orm, texCoord, ddx, ddy).rgb;
        ao = ormSample.r;
        roughness *= ormSample.g; 
        metallic  *= ormSample.b; 
    }
    else
    {
        ao = textureGrad(material.ao, texCoord, ddx, ddy).r;
        roughness *= textureGrad(material.roughness, texCoord, ddx, ddy).r;
        metallic  *= textureGrad(material.metallic, texCoord, ddx, ddy).r;
    }

    // 4. G-Buffer 채우기
    gPosition.rgb = FragPos;
    gPosition.a = ao;

    // [Normal]
    vec3 normalMapValue = textureGrad(material.normal, texCoord, ddx, ddy).rgb;
    
    // NormalMap 0 벡터 방지
    vec3 rawNormal = normalMapValue * 2.0 - 1.0;
    if (dot(rawNormal, rawNormal) < 1e-6) rawNormal = vec3(0, 0, 1);
    normalMapValue = normalize(rawNormal);
    
    gNormal.rgb = SafeNormalize(orthoTBN * normalMapValue);
    
    // ★ [중요] Roughness 0 방지 (Lighting Shader 폭발 방지)
    gNormal.a = max(roughness, 0.05); 

    // [Albedo]
    vec4 albedoSample = textureGrad(material.diffuse, texCoord, ddx, ddy);
    vec3 finalAlbedo = albedoSample.rgb * material.albedoFactor.rgb;

    // Alpha Masking
    if (albedoSample.a * material.albedoFactor.a < 0.1) discard;

    gAlbedoSpec.rgb = finalAlbedo;
    gAlbedoSpec.a = metallic;

    // [Emission]
    vec3 emissionTex = textureGrad(material.emission, texCoord, ddx, ddy).rgb;
    vec3 finalEmission = (emissionTex * material.emissionStrength) + material.emissiveFactor;
    gEmission = vec4(finalEmission, 1.0);

    // [Velocity]
    vec2 currNDC = vCurrClipPos.xy / vCurrClipPos.w;
    vec2 prevNDC = vPrevClipPos.xy / vPrevClipPos.w;
    vec2 velocity = (currNDC - prevNDC) * 0.5;
    gVelocity = velocity;
}