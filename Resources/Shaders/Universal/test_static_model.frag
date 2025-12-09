#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;

// [UBO] Camera Data (Binding 0)
layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

// [UBO] Light Data (Binding 1)
struct LightInfo
{
    vec3 position;    float pad0;
    vec3 direction;   float pad1;
    vec2 cutoff;      vec2  pad2;
    vec3 attenuation; float pad3;
    vec3 ambient;     float pad4;
    vec3 diffuse;     float intensity;
    vec3 specular;    float pad5;
    int type;         int shadowMapIndex; vec2 pad6;
};

#define MAX_LIGHTS 32
layout (std140, binding = 1) uniform LightData
{
    LightInfo lights[MAX_LIGHTS];
    vec3 viewPos_LightUBO; 
    int lightCount;
};

// [Material] C++ Material 클래스와 100% 일치
struct Material 
{
    sampler2D diffuse;      // Slot 0 (Albedo)
    sampler2D specular;     // Slot 1
    sampler2D emission;     // Slot 2
    sampler2D normal;       // Slot 3
    sampler2D height;       // Slot 4
    sampler2D ao;           // Slot 5
    sampler2D metallic;     // Slot 6
    sampler2D roughness;    // Slot 7

    float shininess;      
    float emissionStrength;
    float heightScale;
};
uniform Material material;

const float PI = 3.14159265359;

// --- PBR Helper Functions ---
float DistributionGGX(vec3 normal, vec3 halfDir, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float dotNH = max(dot(normal, halfDir), 0.0);
    float dotNH2 = dotNH * dotNH;
    float denom = (dotNH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float dotNV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return dotNV / (dotNV * (1.0 - k) + k);
}

float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
    float dotNV = max(dot(normal, viewDir), 0.0);
    float dotNL = max(dot(normal, lightDir), 0.0);
    float ggx2 = GeometrySchlickGGX(dotNV, roughness);
    float ggx1 = GeometrySchlickGGX(dotNL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Parallax Mapping
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    // [안전장치] HeightScale이 0이면 계산 스킵 (일렁임 방지)
    if (material.heightScale <= 0.0) return texCoords;

    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(viewDir.z));  
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    
    vec2 P = viewDir.xy / viewDir.z * material.heightScale; 
    vec2 deltaTexCoords = P / numLayers;
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = 1.0 - texture(material.height, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = 1.0 - texture(material.height, currentTexCoords).r;  
        currentLayerDepth += layerDepth;  
    }
    return currentTexCoords;
}

void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);

    // 1. Parallax Mapping (TBN 역행렬 이용)
    mat3 TBN_trans = transpose(TBN);
    vec3 viewDirTangent = normalize(TBN_trans * viewDir);
    
    vec2 uv = TexCoords;
    if (material.heightScale > 0.0)
    {
        uv = ParallaxMapping(TexCoords, viewDirTangent);
        // 패럴랙스로 인해 UV 범위를 벗어나면 버림 (선택 사항)
        if(uv.x > 1.0 || uv.y > 1.0 || uv.x < 0.0 || uv.y < 0.0) discard;
    }

    // 2. 텍스쳐 샘플링
    vec3 albedo     = pow(texture(material.diffuse, uv).rgb, vec3(2.2));
    float metallic  = texture(material.metallic, uv).r;
    float roughness = texture(material.roughness, uv).r;
    float ao        = texture(material.ao, uv).r;
    vec3 emission   = texture(material.emission, uv).rgb;

    // 3. Normal Mapping (GBuffer가 아니므로 여기서 직접 계산)
    vec3 normalMapValue = texture(material.normal, uv).rgb;
    normalMapValue = normalize(normalMapValue * 2.0 - 1.0);
    vec3 N = normalize(TBN * normalMapValue); // [중요] Vertex의 Normal 대신 NormalMap 적용

    // 4. PBR Lighting Calculation
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    vec3 Lo = vec3(0.0);

    for(int i = 0; i < lightCount; ++i) 
    {
        vec3 L;
        float attenuation = 1.0;

        // Directional
        if (lights[i].type == 0) {
            L = normalize(-lights[i].direction);
        } else {
            L = normalize(lights[i].position - FragPos);
            float distance = length(lights[i].position - FragPos);
            attenuation = 1.0 / (lights[i].attenuation.x + lights[i].attenuation.y * distance + lights[i].attenuation.z * distance * distance);
        }

        // Spot Light
        if (lights[i].type == 2) {
            float theta = dot(L, normalize(-lights[i].direction));
            float epsilon = lights[i].cutoff.x - lights[i].cutoff.y;
            float intensity = clamp((theta - lights[i].cutoff.y) / epsilon, 0.0, 1.0);
            attenuation *= intensity;
        }

        vec3 H = normalize(viewDir + L);
        vec3 radiance = lights[i].diffuse * lights[i].intensity * attenuation;

        // BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, viewDir, L, roughness);       
        vec3 F    = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);
            
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, viewDir), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - metallic);   

        float NdotL = max(dot(N, L), 0.0);        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   
    
    vec3 ambient = lights[0].ambient * albedo * ao; // 첫 번째 라이트의 ambient 사용 (임시)
    vec3 color = ambient + Lo + (emission * material.emissionStrength);

    // Tone Mapping & Gamma
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);

    // =================================================================
    // [DEBUG MODE] 아래 주석을 하나씩 풀어서 문제가 되는 텍스쳐를 찾으세요.
    // =================================================================
    
    // FragColor = vec4(uv, 0.0, 1.0);               // 1. UV 좌표 (일렁임 체크)
    // FragColor = vec4(albedo, 1.0);                // 2. Diffuse/Albedo 맵
    // FragColor = vec4(vec3(metallic), 1.0);        // 3. Metallic 맵 (흑백)
    // FragColor = vec4(vec3(roughness), 1.0);       // 4. Roughness 맵 (흑백)
    // FragColor = vec4(normalMapValue, 1.0);        // 5. Normal Map (푸르스름)
    // FragColor = vec4(vec3(ao), 1.0);              // 6. AO 맵
    // FragColor = vec4(TBN[0], 1.0);                // 7. Tangent 벡터 시각화 (빨간색 위주여야 함)
}