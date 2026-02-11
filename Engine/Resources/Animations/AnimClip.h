#pragma once

class AnimClip
{
public:
	AnimClip();
	~AnimClip();

	// 복사 방지
	AnimClip(const AnimClip&) = delete;
	AnimClip& operator=(const AnimClip&) = delete;
	
	// 이동 시맨틱 (데이터가 꽤 크기에 오로지 이동만 허용)
	AnimClip(AnimClip&& other) noexcept;
	AnimClip& operator=(AnimClip&& other) noexcept;

public:
	bool IsValid() const;
	void Clear();

public:
	std::vector<glm::mat4> localMatrices;
	uint32 frameCount = 0;
	uint32 boneCount = 0;
	float frameRate = 30.0f;
};