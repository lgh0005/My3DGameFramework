#pragma once

class TempCamera
{
public:
	TempCamera(glm::mat4 view, glm::mat4 proj);
	~TempCamera() = default;

public:
	void Init();
	void Update();

public:
	glm::mat4 GetView() const { return _view; }
	glm::mat4 GetProjection() const { return _projection; }

private:
	void KeyControl();
	void MouseControl();
	void RecalculateVectors();

private:
	glm::mat4 _view;
	glm::mat4 _projection;

private:
	glm::vec3 _position;    // 카메라의 World 좌표상의 위치
	glm::quat _orientation; // 쿼터니언 회전 저장

	glm::vec3 _front;      // 카메라 좌표계 상의 _front 벡터
	glm::vec3 _up;         // 카메라 좌표계 상의 _up 벡터
	glm::vec3 _right;      // 카메라 좌표계 상의 _right 벡터
	glm::vec3 _worldUp;    // 카메라가 있는 World 좌표상의 Up 벡터.

	GLfloat _yaw;          // 도리도리 : 오른손 좌표계 기준 y축 고정 후 회전
	GLfloat _pitch;        // 끄덕끄덕 : 오른손 좌표계 기준 x축 고정 후 회전

	GLfloat _speed;        // 이동 속도 : 키보드로 인한 움직임의 속도
	GLfloat _turnSpeed;    // 회전 속도 : 마우스로 인한 회전의 속도
};

