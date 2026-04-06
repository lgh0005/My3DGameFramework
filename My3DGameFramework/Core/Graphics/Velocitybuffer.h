#pragma once

CLASS_PTR(Velocitybuffer)
class Velocitybuffer
{
public:
	~Velocitybuffer();
	static VelocitybufferUPtr Create();

	void RecordMovement(const glm::vec3& displacement, float dt);
	glm::vec3 GetSmoothVelocity(float targetDt) const;
	void Reset();

private:
	Velocitybuffer();

	glm::vec3 m_accumulatedDisplacement  { 0.0f };
	float m_accumulatedTime              { 0.0f };
	const float m_maxWindow              { 0.033f };
};