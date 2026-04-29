#pragma once

namespace MGF3D
{
	struct alignas(16) LightInfo
	{
        // [16 bytes] Point/Spot 전용 : 위치 및 감쇄 범위
        vec3  position;
        float intensity;

        // [16 bytes] Spot 전용 (cos 값) : 방향 및 스팟 내각
        vec3  direction;
        float innerCutoff;

        // [16 bytes] Spot 전용 (cos 값) : 색상 및 스팟 외각
        vec3  color;
        float outerCutoff;

        // [16 bytes] 감쇄 계수(kc, kl, kq)와 조명 타입
        vec3  attenuation;
        int32 type; // 0: Directional, 1: Point, 2: Spot

        // [16 bytes] 그림자 인덱스와 나머지 패딩
        int32 shadowMapIndex;
        float pad0; float pad1; float pad2;
	};

    struct alignas(16) LightData
    {
        LightInfo lights[MAX_LIGHTS];
        vec3 viewPos;
        int32 lightCount;
    };
}