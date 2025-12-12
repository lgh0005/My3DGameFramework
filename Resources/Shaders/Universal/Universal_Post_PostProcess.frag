#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float gamma;
uniform float exposure;

void main()
{
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
  
    // 1. Tone Mapping (Exposure ±â¹Ý)
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    
    // 2. Gamma Correction
    result = pow(result, vec3(1.0 / gamma));
  
    FragColor = vec4(result, 1.0);
}