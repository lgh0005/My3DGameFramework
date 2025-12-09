#version 460 core
out vec4 fragColor;

in vec3 localPos;

uniform sampler2D tex;

const vec2 invPi = vec2(0.1591549, 0.3183098862);

vec2 SampleSphericalMap(vec3 v) 
{
    // 이 코드는 구면좌표계를 평면으로 되돌리는 연산이다.
    // 구를 구할 때 u와 v성분을 계산하는 방법을 거꾸로 하고 있다.
    return vec2(atan(v.z, v.x), asin(v.y)) * invPi + 0.5;
}

void main() 
{
    vec2 uv = SampleSphericalMap(normalize(localPos));
    vec3 color = texture(tex, uv).rgb;
    fragColor = vec4(color, 1.0);
}