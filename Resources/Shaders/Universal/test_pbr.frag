#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

// [UBO] Camera Data (Binding 0) - viewPos 가져오기 위함
layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

// [UBO] Light Data (Binding 1) - 구조체 패딩 일치 필수!
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
    vec3 viewPos_LightUBO; // C++ 구조체에 있어서 넣었지만, 위 CameraData의 viewPos 써도 됨
    int lightCount;
};

// [Material] 텍스처 슬롯 기반 (C++ SetToProgram과 매칭)
struct Material {
    sampler2D albedoMap;    // Slot 0
    sampler2D normalMap;    // Slot 3
    sampler2D metallicMap;  // Slot 6
    sampler2D roughnessMap; // Slot 7
    sampler2D aoMap;        // Slot 5
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

void main()
{
    // 1. Material 속성 읽기 (텍스처에서 샘플링)
    // Albedo는 sRGB -> Linear 변환
    vec3 albedo     = pow(texture(material.albedoMap, TexCoords).rgb, vec3(2.2));
    float metallic  = texture(material.metallicMap, TexCoords).r;
    float roughness = texture(material.roughnessMap, TexCoords).r;
    float ao        = texture(material.aoMap, TexCoords).r;

    // Normal Map 처리
    vec3 normValue = texture(material.normalMap, TexCoords).rgb;
    normValue = normalize(normValue * 2.0 - 1.0);
    // (간이 TBN: 실제로는 Vertex Shader에서 TBN을 넘겨받는 게 정확함)
    // 여기서는 일단 World Normal을 그대로 사용한다고 가정하거나,
    // TBN 행렬을 Vertex Shader에 추가해야 함. 
    // *임시*: 노멀맵이 (0.5, 0.5, 1.0) Flat Normal이라면 Mesh Normal 사용
    vec3 N = normalize(Normal); 
    
    vec3 V = normalize(viewPos - FragPos);

    // F0 설정
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    // [UBO 조명 루프]
    for(int i = 0; i < lightCount; ++i) 
    {
        // LightInfo 구조체 사용
        vec3 L;
        float attenuation = 1.0;

        // Directional (Type 0) vs Point/Spot
        if (lights[i].type == 0) {
            L = normalize(-lights[i].direction);
        } else {
            L = normalize(lights[i].position - FragPos);
            float distance = length(lights[i].position - FragPos);
            // 감쇠 공식 (C++과 일치시킬 것)
            attenuation = 1.0 / (lights[i].attenuation.x + lights[i].attenuation.y * distance + lights[i].attenuation.z * distance * distance);
        }

        // Spot Light (Type 2)
        if (lights[i].type == 2) {
            float theta = dot(L, normalize(-lights[i].direction));
            float epsilon = lights[i].cutoff.x - lights[i].cutoff.y;
            float intensity = clamp((theta - lights[i].cutoff.y) / epsilon, 0.0, 1.0);
            attenuation *= intensity;
        }

        vec3 H = normalize(V + L);
        
        // Intensity 적용
        vec3 radiance = lights[i].diffuse * lights[i].intensity * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - metallic);	  

        float NdotL = max(dot(N, L), 0.0);        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   
    
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    // HDR Tone Mapping & Gamma Correction
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}