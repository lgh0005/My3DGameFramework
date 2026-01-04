#version 460 core

// DEBUG
out vec4 FragColor;

void main()
{
    // 깊이 값은 GPU에 의해 자동으로 깊이 버퍼에 기록됩니다.
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}