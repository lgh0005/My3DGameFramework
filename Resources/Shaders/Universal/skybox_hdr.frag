#version 460 core

out vec4 fragColor;
in vec3 localPos;

uniform samplerCube cubeMap;

void main() 
{
    // TEMP : 잠시 lod로 roughness를 통해서 prefilter map을 점검 중
    vec3 envColor = texture(cubeMap, localPos).rgb;
    envColor = envColor / (envColor + vec3(1.0));   // reinhard
    envColor = pow(envColor, vec3(1.0/2.2));    // to sRGB
    fragColor = vec4(envColor, 1.0);
}