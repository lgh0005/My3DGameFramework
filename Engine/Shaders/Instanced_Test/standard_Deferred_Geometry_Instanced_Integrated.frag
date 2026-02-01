#version 460 core

layout (location = 0) out vec4 gPosition;    // RGB: WorldPos, A: AO
layout (location = 1) out vec4 gNormal;      // RGB: Normal, A: Shininess
layout (location = 2) out vec4 gAlbedoSpec;  // RGB: Albedo, A: Specular
layout (location = 3) out vec4 gEmission;    // RGB: Emission
layout (location = 4) out vec4 gVelocity;    // RG: Velocity, B: Depth

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;
in vec4 vCurrClipPos;
in vec4 vPrevClipPos;
in vec4 vMeshColor; // VS에서 넘어온 컬러

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
    sampler2D orm;
    sampler2D ao;
    float shininess;
    float emissionStrength;
    float heightScale;
    vec4  albedoFactor;
    bool  hasORM;
};
uniform Material material;

// 패럴랙스 매핑 (디버깅 시 heightScale을 0으로 두어 비활성화 가능)
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
    float weight = afterDepth / (afterDepth - beforeDepth);
    return prevTexCoords * weight + currentTexCoords * (1.0 - weight);
}

void main() 
{
    vec2 texCoord = TexCoords;

    // 1. 패럴랙스 매핑 적용 여부 결정
    if (material.heightScale > 0.001) 
    {
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 viewDirTangent = normalize(transpose(TBN) * viewDir);
        texCoord = ParallaxMapping(TexCoords, viewDirTangent);
        if (texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0) discard;
    }

    // 2. 샘플링 및 데이터 구성

    // [AO]
    float ao = material.hasORM ? texture(material.orm, texCoord).r : texture(material.ao, texCoord).r;
    gPosition = vec4(FragPos, ao);

     // [Normal]
    vec3 normalMap = texture(material.normal, texCoord).rgb * 2.0 - 1.0;
    gNormal = vec4(normalize(TBN * normalize(normalMap)), material.shininess);

    //  [Albedo] (vMeshColor 결합)
    vec4 albedoSample = texture(material.diffuse, texCoord);
    vec4 finalAlbedo = albedoSample * material.albedoFactor * vMeshColor;
    if (finalAlbedo.a < 0.1) discard;
    gAlbedoSpec = vec4(finalAlbedo.rgb, texture(material.specular, texCoord).r);

    //  [Emission]
    vec3 emission = pow(texture(material.emission, texCoord).rgb, vec3(2.2)) * 5.0;
    gEmission = vec4(emission * material.emissionStrength, 1.0);

    //  [Velocity]
    vec2 currNDC = vCurrClipPos.xy / vCurrClipPos.w;
    vec2 prevNDC = vPrevClipPos.xy / vPrevClipPos.w;
    gVelocity = vec4((currNDC - prevNDC) * 0.5, vCurrClipPos.w, 0.0);
}