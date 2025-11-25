#version 460 core

in vec3 normal;
in vec2 texCoords;
in vec3 position;
in vec4 FragPosLightSpace;
in mat3 TBN;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

struct Light 
{
    vec3 position;
    vec3 direction;
    vec2 cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
uniform vec3 viewPos;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    sampler2D normal;
    sampler2D height;
    float shininess;
    float emissionStrength;
    float heightScale;
};
uniform Material material;
uniform sampler2D shadowMap;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    // 1. 레이어 개수 설정 (Steep과 동일)
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(viewDir.z));  
    
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    
    // P: 전체 이동 벡터
    vec2 P = viewDir.xy / viewDir.z * material.heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // 2. 초기값 설정
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = 1.0 - texture(material.height, currentTexCoords).r;
      
    // 3. Steep Parallax Loop (충돌 지점 찾기)
    while (currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = 1.0 - texture(material.height, currentTexCoords).r;  
        currentLayerDepth += layerDepth;  
    }
    
    // 충돌 직전(prev) 좌표와 깊이 구하기
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // afterDepth: 충돌 후 (현재 위치)에서의 (깊이 맵 값 - 현재 층 깊이) -> 음수
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    
    // beforeDepth: 충돌 전 (이전 위치)에서의 (깊이 맵 값 - 이전 층 깊이) -> 양수
    float beforeDepth = (1.0 - texture(material.height, prevTexCoords).r) - (currentLayerDepth - layerDepth);
 
    // 두 깊이 차이를 이용해 가중치(weight) 계산
    float weight = afterDepth / (afterDepth - beforeDepth);
    
    // 가중치를 이용해 정밀한 UV 좌표 계산 (Linear Interpolation)
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // 1. Perspective Divide (직교 투영이면 w가 1이라 변화 없음)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // 2. [-1, 1] 범위를 [0, 1] 범위로 변환
    projCoords = projCoords * 0.5 + 0.5;

    // 4. 현재 픽셀의 깊이와 바이어스 계산 (Shadow Acne 방지)
    float currentDepth = projCoords.z;
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);

    // 5. PCF (Percentage-Closer Filtering) - 부드러운 그림자
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    return shadow;
}

// TODO
// 1. 광원에 따른 셰이더 필요
void main() 
{
    // Tangent Space View Direction 계산
    vec3 viewDirWorld = normalize(viewPos - position);
    mat3 worldToTangent = transpose(TBN);
    vec3 viewDirTangent = normalize(worldToTangent * viewDirWorld);

    // Parallax Mapping 적용
    vec2 texCoord = texCoords;
    texCoord = ParallaxMapping(texCoords, viewDirTangent);
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    // 조명 연산
	// 1. Ambient
	vec3 texColor = texture(material.diffuse, texCoord).xyz;
    vec3 ambient = texColor * light.ambient;

	// 2. Attenuation and Diffuse
	float dist = length(light.position - position);
    vec3 distPoly = vec3(1.0, dist, dist*dist);
    float attenuation = 1.0 / dot(distPoly, light.attenuation);
    vec3 lightDir = normalize(light.position - position);

    vec3 result = ambient;
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp
    (
        (theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]),
        0.0, 1.0
    );

    if (intensity > 0.0) 
    {
        // 노멀맵에 있는 N벡터를 추출
        vec3 normalMapValue = texture(material.normal, texCoord).rgb;
        normalMapValue = normalMapValue * 2.0 - 1.0;
        vec3 pixelNorm = normalize(TBN * normalMapValue);

        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;

        vec3 specColor = texture2D(material.specular, texCoord).xyz;

        // blinn-phong's specular operation
        vec3 viewDir = normalize(viewPos - position);
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
        vec3 specular = spec * specColor * light.specular;

        float shadow = ShadowCalculation(FragPosLightSpace, pixelNorm, lightDir);
        result += (1.0 - shadow) * (diffuse + specular) * intensity;
    }

    result *= attenuation;

    // emission
    vec3 emission = texture(material.emission, texCoord).rgb * material.emissionStrength;
    result += emission;

	fragColor = vec4(result, 1.0);

    // brightColor 추출
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        brightColor = vec4(result, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}