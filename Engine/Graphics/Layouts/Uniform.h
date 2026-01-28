#pragma once 

struct CameraData
{
    glm::mat4 view;         // 64 bytes
    glm::mat4 projection;   // 64 bytes
    glm::vec3 viewPos;      // 12 bytes
    float pad0;             // 4 bytes (총 144 bytes)
};

struct LightData
{
    struct LightInfo
    {
        glm::vec3 position;     float pad0;
        glm::vec3 direction;    float pad1;
        glm::vec2 cutoff;       glm::vec2 pad2;
        glm::vec3 attenuation;  float pad3;
        glm::vec3 ambient;      float pad4;
        glm::vec3 diffuse;      float intensity;
        glm::vec3 specular;     float pad5;
        int32 type;             
        int32 shadowMapIndex;   glm::vec2 pad6;

    } lights[MAX_LIGHTS];  // 128 * 32 = 4096 bytes
    glm::vec3 viewPos;     // 12 bytes
    int32 lightCount;      // 4 bytes (총 4112 bytes)
};