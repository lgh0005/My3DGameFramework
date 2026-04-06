#version 460 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gEmission;
layout (location = 4) out vec2 gVelocity;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;
in vec4 vCurrClipPos;
in vec4 vPrevClipPos;
in vec4 vMeshColor;

layout (std140, binding = 0) uniform CameraData 
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

struct Material 
{
    sampler2D diffuse, specular, emission, normal, height, orm, ao, metallic, roughness;
    float shininess, emissionStrength, heightScale, roughnessFactor, metallicFactor;
    vec4  albedoFactor;
    vec3  emissiveFactor;
    bool  hasORM;
};
uniform Material material;

vec3 SafeNormalize(vec3 v) { return (dot(v, v) < 1e-6) ? vec3(0, 0, 1) : normalize(v); }
bool IsNaN2(vec2 v) { return v.x != v.x || v.y != v.y; }

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) 
{
    if (viewDir.z < 1e-3) return texCoords;

    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(viewDir.z));
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    
    vec2 P = viewDir.xy / viewDir.z * material.heightScale;
    vec2 deltaTexCoords = P / numLayers;
    vec2 currentTexCoords = texCoords;

    float currentDepthMapValue = 1.0 - textureLod(material.height, currentTexCoords, 0.0).r;
      
    while (currentLayerDepth < currentDepthMapValue) 
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = 1.0 - textureLod(material.height, currentTexCoords, 0.0).r;
        currentLayerDepth += layerDepth;
    }
    
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (1.0 - textureLod(material.height, prevTexCoords, 0.0).r) - (currentLayerDepth - layerDepth);
    
    float denominator = afterDepth - beforeDepth;
    float weight = (abs(denominator) > 1e-5) ? afterDepth / denominator : 0.0;
    return prevTexCoords * weight + currentTexCoords * (1.0 - weight);
}

void main() 
{
    // 1. 밉맵 보정을 위한 미분값 저장
    vec2 ddx = dFdx(TexCoords);
    vec2 ddy = dFdy(TexCoords);

    // 2. TBN 및 패럴랙스 매핑
    mat3 orthoTBN = mat3(SafeNormalize(TBN[0]), SafeNormalize(TBN[1]), SafeNormalize(TBN[2]));
    vec2 texCoord = TexCoords;

    if (material.heightScale > 0.001) {
        vec3 viewDir = SafeNormalize(viewPos - FragPos);
        vec3 viewDirTangent = SafeNormalize(transpose(orthoTBN) * viewDir);
        texCoord = ParallaxMapping(TexCoords, viewDirTangent);
        if (IsNaN2(texCoord) || texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0) discard;
    }

    // 3. PBR 파라미터 샘플링 (textureGrad 사용)
    float ao = 1.0, roughness = material.roughnessFactor, metallic = material.metallicFactor;
    if (material.hasORM) {
        vec3 orm = textureGrad(material.orm, texCoord, ddx, ddy).rgb;
        ao = orm.r; roughness *= orm.g; metallic *= orm.b;
    } else {
        ao = textureGrad(material.ao, texCoord, ddx, ddy).r;
        roughness *= textureGrad(material.roughness, texCoord, ddx, ddy).r;
        metallic *= textureGrad(material.metallic, texCoord, ddx, ddy).r;
    }

    // 4. G-Buffer 채우기
    gPosition = vec4(FragPos, ao);

    vec3 normalMap = textureGrad(material.normal, texCoord, ddx, ddy).rgb * 2.0 - 1.0;
    if (dot(normalMap, normalMap) < 1e-6) normalMap = vec3(0, 0, 1);
    gNormal = vec4(SafeNormalize(orthoTBN * normalize(normalMap)), max(roughness, 0.05));

    vec4 albedoSample = textureGrad(material.diffuse, texCoord, ddx, ddy);
    vec4 finalAlbedo = albedoSample * material.albedoFactor * vMeshColor;
    if (finalAlbedo.a < 0.1) discard;

    gAlbedoSpec = vec4(finalAlbedo.rgb, metallic);
    gEmission = vec4((textureGrad(material.emission, texCoord, ddx, ddy).rgb * material.emissionStrength) + material.emissiveFactor, 1.0);

    // 5. 벨로시티 (모션 벡터)
    gVelocity = (vCurrClipPos.xy / vCurrClipPos.w - vPrevClipPos.xy / vPrevClipPos.w) * 0.5;
}