#version 460 core

in vec3 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() 
{
    // 광원과 현재 프래그먼트(픽셀) 사이의 거리 계산
    float lightDistance = length(FragPos - lightPos);

    // 거리를 [0, 1] 범위로 선형 정규화
    lightDistance = lightDistance / farPlane;

    // 계산된 선형 거리를 깊이 버퍼에 직접 기록
    gl_FragDepth = lightDistance;
}