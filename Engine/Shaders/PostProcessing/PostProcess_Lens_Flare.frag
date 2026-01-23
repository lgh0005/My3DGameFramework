#version 460 core

out vec4 fragColor;
in vec2 TexCoord;

uniform sampler2D uSceneTex;
uniform sampler2D uBloomTex;

uniform int   uGhostCount;
uniform float uGhostSpacing;
uniform float uHaloRadius;
uniform float uFlareIntensity;
uniform vec2  uTexelSize;

// 1. 가장자리 아티팩트 방지용 마스크
float GetEdgeMask(vec2 uv) 
{
    vec2 edge = smoothstep(0.0, 0.15, uv) * smoothstep(1.0, 0.85, uv);
    return edge.x * edge.y;
}

// 2. 색수차가 적용된 샘플링 함수 (이미지 왜곡 및 클램핑 포함)
vec3 textureDistorted(sampler2D tex, vec2 uv, vec2 dir, vec3 distortion) 
{
    float mask = GetEdgeMask(uv);
    vec3 color = vec3(
        texture(tex, clamp(uv + dir * distortion.r, 0.0, 1.0)).r,
        texture(tex, clamp(uv + dir * distortion.g, 0.0, 1.0)).g,
        texture(tex, clamp(uv + dir * distortion.b, 0.0, 1.0)).b
    );
    return color * mask;
}

void main()
{
    vec3 sceneColor = texture(uSceneTex, TexCoord).rgb;
    float aspect = uTexelSize.y / uTexelSize.x;

    // [핵심 1] 소프트 노멀라이제이션
    // 중앙(0.5, 0.5)에서 분모가 0이 되어 방향이 튀는 것을 방지하기 위해 미세한 epsilon을 더합니다.
    vec2 toCenter = vec2(0.5) - TexCoord;
    float distToCenter = length(toCenter * vec2(aspect, 1.0));
    vec2 dir = toCenter / (distToCenter + 0.001); 

    vec3 distortion = vec3(-uTexelSize.x * 3.0, 0.0, uTexelSize.x * 3.0);
    vec3 resultFlare = vec3(0.0);

    // [단계 1] 고스트 (Ghosts)
    for (int i = 0; i < uGhostCount; ++i)
    {
        vec2 offset = TexCoord + toCenter * (uGhostSpacing * float(i));
        float d = length((vec2(0.5) - offset) * vec2(aspect, 1.0));
        float weight = pow(1.0 - clamp(d / 0.7, 0.0, 1.0), 4.0);
        resultFlare += textureDistorted(uBloomTex, offset, dir, distortion) * weight;
    }

    // [단계 2] 헤일로 (Halo) - 핀치 현상 및 싱귤러리티 최종 수정
    // 중앙 뮤트 범위를 조금 더 넓게 잡아 자연스러운 소멸 유도
    float haloCenterMute = smoothstep(0.05, 0.35, distToCenter);
    
    // [핵심 2] 무지갯빛 할로 (Chroma Halo)
    // 단일 샘플링 대신 R, G, B 채널에 오프셋을 주어 링의 경계를 부드럽게 뭉갭니다.
    for(int j = 0; j < 3; ++j)
    {
        // 채널별로 반지름을 미세하게 다르게 설정 (굴절률 차이 모사)
        float chromaRadius = uHaloRadius + (float(j) - 1.0) * 0.005;
        vec2 haloSampleCoord = TexCoord + dir * chromaRadius;

        // 링의 가중치 계산
        vec2 haloDelta = (vec2(0.5) - haloSampleCoord) * vec2(aspect, 1.0);
        float haloDist = length(haloDelta);
        float haloWeight = pow(1.0 - clamp(abs(haloDist - uHaloRadius) * 8.0, 0.0, 1.0), 12.0);

        vec3 sampledBloom = textureDistorted(uBloomTex, haloSampleCoord, dir, distortion);
        
        // 결과 채널별 합성
        if(j == 0) resultFlare.r += sampledBloom.r * haloWeight;
        if(j == 1) resultFlare.g += sampledBloom.g * haloWeight;
        if(j == 2) resultFlare.b += sampledBloom.b * haloWeight;
    }

    // 최종 합성
    fragColor = vec4(sceneColor + (resultFlare * uFlareIntensity * haloCenterMute), 1.0);
}