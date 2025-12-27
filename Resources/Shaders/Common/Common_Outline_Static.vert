#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

uniform mat4 model;
uniform float outlineThickness;

void main()
{
    // 1. 월드 공간으로 변환
    vec4 worldPos = model * vec4(aPos, 1.0);
    
    // 2. 월드 기준 노멀 계산
    vec3 worldNormal = normalize(mat3(transpose(inverse(model))) * aNormal);

    // 3. 노멀 방향으로 정점 밀어내기 (Extrusion)
    worldPos.xyz += worldNormal * outlineThickness;
    gl_Position = projection * view * worldPos;
}