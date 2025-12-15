#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal; // Vertex Normal (Fallback용)
in mat3 TBN;    // [New] TBN 행렬

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
struct Material 
{
    sampler2D diffuse;
    sampler2D normal;
    sampler2D emission;     // [New]
    sampler2D specular;     // (Slot Dummy)
    sampler2D height;       // (Slot Dummy)

    sampler2D orm;          // [New] ORM
    sampler2D ao;           
    sampler2D metallic;     
    sampler2D roughness;    

    float shininess;        
    float emissionStrength; // [New]
    float heightScale;      
    
    vec4  albedoFactor;     // [New]
    float metallicFactor;   // [New]
    float roughnessFactor;  // [New]

    bool hasORM;            // [New]
    bool useGlossiness;     // [New]
};
uniform Material material;

layout(binding = 10) uniform samplerCube irradianceMap;
layout(binding = 11) uniform samplerCube prefilterMap;
layout(binding = 12) uniform sampler2D   brdf;
uniform int useIBL; // 잠시 차이를 비교하기 위한 임시 유니폼

const float PI = 3.14159265359;

// --- PBR Helper Functions ---
float DistributionGGX(vec3 normal, vec3 halfDir, float roughness) 
{
    float a = roughness * roughness;
    float a2 = a * a;
    float dotNH = max(dot(normal, halfDir), 0.0);
    float dotNH2 = dotNH * dotNH;
    float denom = (dotNH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float dotNV, float roughness) 
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return dotNV / (dotNV * (1.0 - k) + k);
}

float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) 
{
    float dotNV = max(dot(normal, viewDir), 0.0);
    float dotNL = max(dot(normal, lightDir), 0.0);
    float ggx2 = GeometrySchlickGGX(dotNV, roughness);
    float ggx1 = GeometrySchlickGGX(dotNL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) 
{
     return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) 
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// --- PBR Helper Functions --- 

void main()
{
    // 1. Material 속성 추출
    // Albedo는 sRGB -> Linear 변환
    vec4 albedoSample = texture(material.diffuse, TexCoords);
    vec3 albedo = pow(albedoSample.rgb, vec3(2.2)) * material.albedoFactor.rgb;
    float alpha = albedoSample.a * material.albedoFactor.a;
    if (alpha < 0.1) discard;

    // 2. orm 처리 및 factor 적용
    float metallic, roughness, ao;
    if (material.hasORM)
    {
        vec4 ormSample = texture(material.orm, TexCoords);
        ao        = ormSample.r;
        roughness = ormSample.g;
        metallic  = ormSample.b;
    }
    else
    {
        ao = texture(material.ao, TexCoords).r;
        metallic = texture(material.metallic, TexCoords).r;
        float rSample = texture(material.roughness, TexCoords).r;
        if (material.useGlossiness) roughness = 1.0 - rSample;
        else roughness = rSample;
    }
    metallic  *= material.metallicFactor;
    roughness *= material.roughnessFactor;

    // 3. Normal Map 처리
    vec3 normValue = texture(material.normal, TexCoords).rgb;
    normValue = normalize(normValue * 2.0 - 1.0);
    // vec3 N = normalize(TBN * normValue);
    // [DEBUG]
    vec3 N = normalize(Normal);

    // 4. PBR 조명 계산
    vec3 V = normalize(viewPos - FragPos);
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    // [Direct Lighting Loop]
    for (int i = 0; i < lightCount; ++i)
    {
        // LightInfo 구조체 사용
        vec3 L;
        float attenuation = 1.0;

        // 조명 타입별 L을 계산
        // (Directional) && (Point, Spot)을 각각 처리
        if (lights[i].type == 0)
        {
            // Directional 조명인 경우
            L = normalize(-lights[i].direction);
        }
        else
        {
            // 그 외의 조명 : Point, Spot 조명인 경우
            // 감쇠(attenuation)를 계산
            L = normalize(lights[i].position - FragPos);
            float dist = length(lights[i].position - FragPos);
            attenuation = 1.0 / (lights[i].attenuation.x + lights[i].attenuation.y 
                * dist + lights[i].attenuation.z * dist * dist);
        }

        // Sopt 조명인 경우에 대한 추가 처리
        if (lights[i].type == 2)
        {
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
        vec3 specular     = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= (1.0 - metallic);      

        float NdotL = max(dot(N, L), 0.0);        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // [Ambient Lighting: IBL vs Constant]
    vec3 ambient = vec3(0.0);
    if (useIBL > 0) 
    {
        vec3 kS = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
        vec3 kD = 1.0 - kS;
        kD *= (1.0 - metallic);
        
        vec3 irradiance = texture(irradianceMap, N).rgb;
        vec3 diffuse    = irradiance * albedo;
        
        const float MAX_REFLECTION_LOD = 4.0; 
        vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 brdf  = texture(brdf, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);
        
        ambient = (kD * diffuse + specular) * ao;
    }
    else
    {
        ambient = vec3(0.03) * albedo * ao;
    }

    // [Emission]
    vec3 emission = texture(material.emission, TexCoords).rgb;
    // emission = pow(emission, vec3(2.2)); // sRGB라면 필요
    emission *= material.emissionStrength;

    // 3. 최종 색상 합성
    vec3 color = ambient + Lo + emission;
    FragColor = vec4(color, alpha);

    // 포스트-프로세싱 패스에서 톤 매핑과 감마 보정을 수행
}