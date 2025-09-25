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
	glm::vec3 _position;    // ī�޶��� World ��ǥ���� ��ġ
	glm::quat _orientation; // ���ʹϾ� ȸ�� ����

	glm::vec3 _front;      // ī�޶� ��ǥ�� ���� _front ����
	glm::vec3 _up;         // ī�޶� ��ǥ�� ���� _up ����
	glm::vec3 _right;      // ī�޶� ��ǥ�� ���� _right ����
	glm::vec3 _worldUp;    // ī�޶� �ִ� World ��ǥ���� Up ����.

	GLfloat _yaw;          // �������� : ������ ��ǥ�� ���� y�� ���� �� ȸ��
	GLfloat _pitch;        // �������� : ������ ��ǥ�� ���� x�� ���� �� ȸ��

	GLfloat _speed;        // �̵� �ӵ� : Ű����� ���� �������� �ӵ�
	GLfloat _turnSpeed;    // ȸ�� �ӵ� : ���콺�� ���� ȸ���� �ӵ�
};

