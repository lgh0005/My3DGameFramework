#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;

// Camera UBO (기존 코드 유지)
layout (std140, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};

uniform mat4 model;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz; 
    TexCoords = aTexCoord;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    
    vec3 T = aTangent;
    if (length(T) < 0.001) T = vec3(1.0, 0.0, 0.0);
    
    T = normalize(normalMatrix * T);
    vec3 N = normalize(normalMatrix * aNormal);
    
    // Gram-Schmidt
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    Normal = N;
    TBN = mat3(T, B, N);
    
    gl_Position = projection * view * worldPos;
}