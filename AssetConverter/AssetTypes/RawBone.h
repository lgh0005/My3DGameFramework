#pragma once

#pragma region BONE_STRUCTS
struct RawBoneInfo
{
    uint32 id;
    glm::mat4 offset;
};

struct RawKeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct RawKeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct RawKeyScale
{
    glm::vec3 scale;
    float timeStamp;
};
#pragma endregion

struct RawBone 
{
    std::string name;
    int32 id;
    glm::mat4 localTransform;

    // 키프레임 데이터들
    std::vector<RawKeyPosition> positions;
    std::vector<RawKeyRotation> rotations;
    std::vector<RawKeyScale>    scales;

    // 계층 구조 저장이 필요하다면 자식 인덱스나 이름을 저장
    std::vector<std::string> childrenNames;
};