#version 460 core

// TODO : 인풋과 아웃풋은 슬슬 하나의 묶음으로 처리
// 할 필요가 있다. vs_in, vs_out, fs_in, fs_out 등과 같이.

layout (location = 0) out vec4 gPosition;      // Attachment 0
layout (location = 1) out vec4 gNormal;        // Attachment 1
layout (location = 2) out vec4 gAlbedoSpec;    // Attachment 2 (RGB: Albedo, A: Specular)
layout (location = 3) out vec4 gEmission;      // Attachment 3
layout (location = 4) out vec4 gVelocity;      // Attachment 4: 속도 벡터

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

    sampler2D orm;
    sampler2D ao;      

    float shininess;      
    float emissionStrength;
    float heightScale;

    vec4  albedoFactor;
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
    
    // [핵심 2] HeightScale이 아주 작으면 아예 패럴랙스 스킵
    // C++에서 HeightMap이 없으면 heightScale을 0.0으로 보내야 함!
    if (material.heightScale > 0.001)
    {
        vec3 viewDir = normalize(viewPos - FragPos);
        mat3 worldToTangent = transpose(orthoTBN);
        vec3 viewDirTangent = normalize(worldToTangent * viewDir);
        
        // 위에서 고친 함수 호출
        texCoord = ParallaxMapping(TexCoords, viewDirTangent);
        
        // 만약 UV가 범위를 벗어나면 버리는 로직 (선택사항)
        // if(texCoord.x > 1.0 || texCoord.y > 1.0 || texCoord.x < 0.0 || texCoord.y < 0.0) discard;
    }

    // 2. 텍스처 샘플링
    // 2-1. AO 맵 샘플링 (Legacy 모드에서도 AO는 쓸 거니까 읽습니다!)
    // 1. AO
    float ao = 1.0;
    if (material.hasORM)
        ao = textureGrad(material.orm, texCoord, ddx, ddy).r; // texture -> textureGrad
    else
        ao = textureGrad(material.ao, texCoord, ddx, ddy).r;  // texture -> textureGrad

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
    vec4 albedoSample = texture(material.diffuse, texCoord);
    vec3 finalAlbedo = albedoSample.rgb * material.albedoFactor.rgb;
    float finalAlpha = albedoSample.a * material.albedoFactor.a;
    if (finalAlpha < 0.1) discard;
    gAlbedoSpec.rgb = finalAlbedo;
    gAlbedoSpec.a = texture(material.specular, texCoord).r;

    // [gEmission] Emission 색상 저장
    // TODO : emission 파라미터 설정 필요
    vec3 emissionTex = texture(material.emission, texCoord).rgb;
    // (선택: 감마보정은 텍스처 로드시 sRGB 옵션 켜는 게 정석)
    // 즉, 혹시 텍스쳐 인코딩이 sRGB가 아닌 것으로 되어있는 지를 먼저 확인 필요
    emissionTex = pow(emissionTex, vec3(2.2));  // 감마 보정
    emissionTex = emissionTex * 5.0;           // 강도 뻥튀기
    gEmission = vec4(emissionTex * material.emissionStrength, 1.0);

    // [gVelocity] Velocity Map
    // RG : Velocity
    // B  : Depth (of main camera)
    vec2 currNDC = vCurrClipPos.xy / vCurrClipPos.w;
    vec2 prevNDC = vPrevClipPos.xy / vPrevClipPos.w;
    vec2 velocity = (currNDC - prevNDC) * 0.5;
    float linearDepth = vCurrClipPos.w;
    gVelocity.xy = velocity;
    gVelocity.z = linearDepth; // 여기에 깊이 저장!
    gVelocity.w = 0.0;         // 예비용
}