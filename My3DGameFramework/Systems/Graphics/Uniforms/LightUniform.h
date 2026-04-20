#pragma once

namespace MGF3D
{
	struct alignas(16) LightInfo
	{
        vec3 position;     float pad0;
        vec3 direction;    float pad1;
        vec2 cutoff;       vec2  pad2;
        vec3 attenuation;  float pad3;
        vec3 ambient;      float pad4;
        vec3 diffuse;      float intensity;
        vec3 specular;     float pad5;
        int32 type;
        int32 shadowMapIndex;   vec2 pad6;
	};

    struct alignas(16) LightData
    {
        LightInfo lights[MAX_LIGHTS];
        vec3 viewPos;
        int32 lightCount;
    };
}