#pragma once 

#define UBO_POINT_CAMERA 0
#define UBO_POINT_LIGHT  1
#define UBO_POINT_SHADOW 2

struct CameraData
{
    glm::mat4 view;         // 64 bytes
    glm::mat4 projection;   // 64 bytes
    glm::vec3 viewPos;      // 12 bytes
    float padding;          // 4 bytes (총 144 bytes)
};

// TODO : 다중 조명에 대한 대응책이 필요.
// 세 광원에 대한 공통 유니폼들을 추가해줄 필요가 있음
// 그 외의 것들은 유니폼 변수로 대체하거나 여기에 추가할 수 있음
struct LightData
{
    glm::vec3 position;     float pad0; // 16 bytes
    glm::vec3 direction;    float pad1; // 16 bytes
    glm::vec2 cutoff;       glm::vec2 pad2; // 16 bytes (vec2는 8byte지만 std140에서 vec3/vec4 정렬 영향 받을 수 있음 -> 넉넉하게)
    glm::vec3 attenuation;  float pad3; // 16 bytes
    glm::vec3 ambient;      float pad4; // 16 bytes
    glm::vec3 diffuse;      float pad5; // 16 bytes
    glm::vec3 specular;     float pad6; // 16 bytes
    // 총 112 bytes
};

struct ShadowData
{
    glm::mat4 lightSpaceMatrix; // 64 bytes
};