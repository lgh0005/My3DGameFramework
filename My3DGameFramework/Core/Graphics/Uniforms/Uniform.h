#pragma once 

namespace MGF3D
{
    struct alignas(16) CameraData
    {
        mat4 view;         // 64 bytes
        mat4 projection;   // 64 bytes
        vec3 viewPos;      // 12 bytes
        float pad0;             // 4 bytes (총 144 bytes)
    };

    struct alignas(16) LightData
    {
        struct alignas(16) LightInfo
        {
            vec3 position;     float pad0;
            vec3 direction;    float pad1;
            vec2 cutoff;       vec2 pad2;
            vec3 attenuation;  float pad3;
            vec3 ambient;      float pad4;
            vec3 diffuse;      float intensity;
            vec3 specular;     float pad5;
            int32 type;
            int32 shadowMapIndex;   vec2 pad6;

        } lights[MAX_LIGHTS];  // 128 * 32 = 4096 bytes
        vec3 viewPos;     // 12 bytes
        int32 lightCount;      // 4 bytes (총 4112 bytes)
    };
}