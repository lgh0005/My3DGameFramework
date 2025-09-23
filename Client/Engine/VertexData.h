#pragma once

struct VertexData
{
	glm::vec3 position;
};

struct VertexTextureData
{
	glm::vec3 position;
	glm::vec2 uv;
};

struct VertexColorData
{
	glm::vec3 position;
	glm::vec4 color;
};