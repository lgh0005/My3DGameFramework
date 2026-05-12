#version 460 core
out float FragColor;

in vec2 TexCoords;

// 모던 OpenGL 바인딩
layout(binding = 0) uniform sampler2D gPositionAO;
layout(binding = 1) uniform sampler2D gNormalRoughness;
layout(binding = 2) uniform sampler2D texNoise;

layout(std140, binding = 0) uniform GlobalUniforms 
{
    mat4 view;               // 64 bytes (offset 0)
    mat4 projection;         // 64 bytes (offset 64)
    vec3 viewPos;            // 12 bytes (offset 128)
    
    float time;              // 4 bytes  (offset 140)
    
    int dirLightCount;       // 4 bytes  (offset 144)
    int pointLightCount;     // 4 bytes  (offset 148)
    int spotLightCount;      // 4 bytes  (offset 152)
    int pad0;                // 4 bytes  (offset 156)
} global;

layout(std140, binding = 2) uniform SSAOKernel { vec4 samples[64]; } uKernel;

const int kernelSize = 64;
const float radius = 0.5;
const float bias = 0.025;

void main()
{
    vec2 noiseScale = textureSize(gPositionAO, 0) / 4.0;
    
    vec3 WorldPos = texture(gPositionAO, TexCoords).xyz;
    if (length(WorldPos) < 0.0001) 
    {
        FragColor = 1.0;
        return;
    }

    // 1. global.view를 사용하여 View Space 좌표로 변환
    vec3 fragPos = (global.view * vec4(WorldPos, 1.0)).xyz;
    vec3 WorldNormal = texture(gNormalRoughness, TexCoords).xyz;
    vec3 normal = mat3(global.view) * WorldNormal;

    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    float currentDepth = fragPos.z;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 samplePos = TBN * uKernel.samples[i].xyz;
        samplePos = fragPos + samplePos * radius; 
        if (samplePos.z > -0.01) continue;

        // 2. 샘플 위치를 화면 좌표(UV)로 투영
        vec4 offset = vec4(samplePos, 1.0);
        offset = global.projection * offset;    
        offset.xyz /= offset.w;                 
        offset.xyz = offset.xyz * 0.5 + 0.5;    
        
        // 3. 샘플링된 지점의 실제 지형 깊이(View Z) 가져오기
        vec3 neighborWorldPos = texture(gPositionAO, offset.xy).xyz;
        if (length(neighborWorldPos) < 0.0001) continue;
        float sampleDepth = (global.view * vec4(neighborWorldPos, 1.0)).z;

        // 4. 차폐 검사 (Standard Comparison)
        float check = (sampleDepth >= samplePos.z + bias) ? 1.0 : 0.0;
        float dist = abs(currentDepth - sampleDepth);
        float rangeCheck = exp(-dist * dist / radius);
        
        // 5. 차폐 처리
        occlusion += check * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / kernelSize);
    occlusion = pow(occlusion, 2.0);
    
    FragColor = occlusion;
}