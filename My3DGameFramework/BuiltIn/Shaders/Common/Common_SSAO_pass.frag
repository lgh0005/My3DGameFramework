#version 460 core
out float FragColor;

in vec2 TexCoords;

// C++에서 바인딩한 텍스처들
uniform sampler2D gPosition; // World Position (필수)
uniform sampler2D gNormal;   // World Normal (필수)
uniform sampler2D texNoise;  // 4x4 Noise Texture

uniform vec3 samples[64];    // Kernel Samples
uniform mat4 projection;     // Projection Matrix
uniform mat4 view;           // Projection view

// SSAO 파라미터 (필요하면 Uniform으로 빼서 런타임 조절 가능)
const int kernelSize = 64;
const float radius = 0.5;    // 차폐 검사 반경
const float bias = 0.025;    // 깊이 정밀도 문제 해결을 위한 바이어스

void main()
{
    // 1. 화면 해상도에 따른 노이즈 타일링 크기 계산
    // texNoise가 4x4 크기이므로 화면 크기를 4로 나눕니다.
    vec2 noiseScale = textureSize(gPosition, 0) / 4.0;

    // 2. G-Buffer 정보 읽기
    vec3 WorldPos = texture(gPosition, TexCoords).xyz;
    if (length(WorldPos) < 0.0001) 
    {
        FragColor = 1.0;
        return;
    }

    vec3 fragPos = (view * vec4(WorldPos, 1.0)).xyz;

    vec3 WorldNormal = texture(gNormal, TexCoords).rgb;
    vec3 normal = mat3(view) * WorldNormal;

    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz;

    // 3. TBN 행렬 생성 (Tangent-Bitangent-Normal)
    // 그람-슈미트 과정을 통해 normal과 randomVec을 직교화합니다.
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    
    // 4. 커널 샘플링 루프
    for(int i = 0; i < kernelSize; ++i)
    {
        // 샘플 위치를 TBN을 이용해 View Space로 변환
        vec3 samplePos = TBN * samples[i]; 
        samplePos = fragPos + samplePos * radius; 
        
        // 5. 샘플 위치를 Screen Space(UV)로 투영
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;    // View -> Clip
        offset.xyz /= offset.w;          // Perspective Divide -> NDC
        offset.xyz = offset.xyz * 0.5 + 0.5; // NDC(-1~1) -> UV(0~1)
        
        // 6. 실제 깊이값 가져오기 (해당 위치에 실제 그려진 물체의 깊이)
        // float sampleDepth = texture(gPosition, offset.xy).z;
        vec3 neighborWorldPos = texture(gPosition, offset.xy).xyz;
        if (length(neighborWorldPos) < 0.0001) continue;
        vec3 neighborViewPos = (view * vec4(neighborWorldPos, 1.0)).xyz;
        float sampleDepth = neighborViewPos.z;
        
        // 7. 차폐 검사
        // sampleDepth(실제 물체)가 samplePos(가상 샘플)보다 앞에 있으면(z값이 크면) 차폐됨
        // 범위 검사 (Range Check): 너무 먼 물체가 차폐하는 현상 방지
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        if (sampleDepth >= samplePos.z + bias)
            occlusion += 1.0 * rangeCheck;
    }
    
    occlusion = 1.0 - (occlusion / kernelSize);
    occlusion = pow(occlusion, 4.0);
    
    FragColor = occlusion;
}

