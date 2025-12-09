#version 460 core

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
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    sampler2D normal;
    sampler2D height;   
    sampler2D ao;      
    sampler2D metallic;
    sampler2D roughness;

    float shininess;      
    float emissionStrength;
    float heightScale;
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
    float currentDepthMapValue = 1.0 - texture(material.height, currentTexCoords).r;
      
    while (currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = 1.0 - texture(material.height, currentTexCoords).r;  
        currentLayerDepth += layerDepth;  
    }
    
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = (1.0 - texture(material.height, prevTexCoords).r) - (currentLayerDepth - layerDepth);
 
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main()
{
    vec2 texCoord = TexCoords;

    // 1. TBN 재정렬 및 패럴랙스 매핑
    vec3 T = normalize(TBN[0]);
    vec3 B = normalize(TBN[1]);
    vec3 N = normalize(TBN[2]);
    mat3 orthoTBN = mat3(T, B, N);
    if (material.heightScale > 0.0)
    {
        vec3 viewDir = normalize(viewPos - FragPos);
        
        // 찌그러진 TBN 대신, 복구된 orthoTBN을 사용해야 정확한 방향이 나옵니다.
        mat3 worldToTangent = transpose(orthoTBN);
        vec3 viewDirTangent = normalize(worldToTangent * viewDir);
        texCoord = ParallaxMapping(TexCoords, viewDirTangent);
    }

    // 2. 텍스처 샘플링
    // AO 맵 샘플링 (Legacy 모드에서도 AO는 쓸 거니까 읽습니다!)
    float ao = texture(material.ao, texCoord).r;

    // 3. G-Buffer 데이터 저장
    // RGB: 월드 좌표
    // A: Ambient Occlusion (AO)
    gPosition.rgb = FragPos;
    gPosition.a = ao;

    // [gNormal]
    // RGB: 노멀
    // A: Shininess (Legacy용)
    vec3 normalMapValue = texture(material.normal, texCoord).rgb;
    normalMapValue = normalize(normalMapValue * 2.0 - 1.0);
    gNormal.rgb = normalize(orthoTBN * normalMapValue);
    gNormal.a = material.shininess;

    // [gAlbedoSpec]
    // RGB: Albedo
    // A: Specular Intensity (Legacy용)
    gAlbedoSpec.rgb = texture(material.diffuse, texCoord).rgb;
    gAlbedoSpec.a = texture(material.specular, texCoord).r;

    // [gEmission] Emission 색상 저장
    // TODO : emission 파라미터 설정 필요
    vec3 emissionTex = texture(material.emission, texCoord).rgb;
    emissionTex = pow(emissionTex, vec3(2.2)); // 감마 보정
    emissionTex = emissionTex * 5.0;           // 강도 뻥튀기
    gEmission = vec4(emissionTex * material.emissionStrength, 1.0);
}