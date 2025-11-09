#version 460 core

in vec2 TexCoords; // 버텍스 셰이더에서 받음
out vec4 fragColor;

// C++ (Material::SetToProgram)에서 바인딩할 디퓨즈 텍스처
struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

// [수정] 2. 'material'이라는 이름으로 struct 유니폼을 선언합니다.
uniform Material material;

void main()
{
    // 텍스처에서 색상을 샘플링하여 그대로 출력
    fragColor = texture(material.diffuse, TexCoords);
    
    // 텍스처 로딩이 실패했거나 알파 채널이 0인 경우
    // 실루엣을 볼 수 있도록 흰색으로 출력 (디버깅용)
    if (fragColor.a < 0.1)
        fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}