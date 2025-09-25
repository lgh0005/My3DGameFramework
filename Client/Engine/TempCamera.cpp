#include "pch.h"
#include "TempCamera.h"

TempCamera::TempCamera(glm::mat4 view, glm::mat4 proj)
	: _view(view), _projection(proj),

    _position(0.f, 0.f, 3.f),
    _front(0.f, 0.f, -1.f),
    _up(0.f, 1.f, 0.f),
    _worldUp(0.f, 1.f, 0.f),
    _yaw(0.f),
    _pitch(0.f),
    _speed(5.f),
    _turnSpeed(0.1f)
{
    // Euler → Quaternion 변환
    glm::quat qPitch = glm::angleAxis(glm::radians(_pitch), glm::vec3(1, 0, 0));
    glm::quat qYaw = glm::angleAxis(glm::radians(_yaw), glm::vec3(0, 1, 0));

    _orientation = glm::normalize(qYaw * qPitch);

    RecalculateVectors();
}

void TempCamera::Init()
{

}

void TempCamera::Update()
{
    // Get Input
    KeyControl();
    MouseControl();

    _view = glm::lookAt(_position, _position + _front, _up);
}

void TempCamera::KeyControl()
{
    float moveSpeed = _speed * TIME.deltaTime;

    glm::vec3 forward = glm::normalize(glm::vec3(_front.x, 0.0f, _front.z));
    glm::vec3 right = glm::normalize(glm::cross(forward, _worldUp));

    if (INPUT.GetKey(Inputs::Key::W)) _position += forward * moveSpeed;
    if (INPUT.GetKey(Inputs::Key::S)) _position -= forward * moveSpeed;
    if (INPUT.GetKey(Inputs::Key::A)) _position -= right * moveSpeed;
    if (INPUT.GetKey(Inputs::Key::D)) _position += right * moveSpeed;
}

/* Eulered */
//void TempCamera::MouseControl()
//{
//    glm::vec2 delta = INPUT.GetMouseDelta();
//
//    _yaw += delta.x * _turnSpeed;
//    _pitch -= delta.y * _turnSpeed;
//
//    if (_pitch > 89.f) _pitch = 89.f;
//    if (_pitch < -89.f) _pitch = -89.f;
//
//    RecalculateVectors();
//}
//
//void TempCamera::RecalculateVectors()
//{
//    glm::vec3 front;
//    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
//    front.y = sin(glm::radians(_pitch));
//    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
//    _front = glm::normalize(front);
//
//    _right = glm::normalize(glm::cross(_front, _worldUp));
//    _up = glm::normalize(glm::cross(_right, _front));
//}

/* Quated */
void TempCamera::MouseControl()
{
    glm::vec2 delta = INPUT.GetMouseDelta();

    _pitch -= delta.y * _turnSpeed;
    _yaw += -delta.x * _turnSpeed;

    // 피치 제한: ±89° 이상 회전 방지
    _pitch = glm::clamp(_pitch, -89.f, 89.f);

    // 쿼터니언 회전 생성
    glm::quat qPitch = glm::angleAxis(glm::radians(_pitch), glm::vec3(1, 0, 0));
    glm::quat qYaw = glm::angleAxis(glm::radians(_yaw), glm::vec3(0, 1, 0));

    _orientation = glm::normalize(qYaw * qPitch);

    RecalculateVectors();
}

void TempCamera::RecalculateVectors()
{
    _front = glm::normalize(_orientation * glm::vec3(0.f, 0.f, -1.f));
    _right = glm::normalize(glm::cross(_front, _worldUp));
    _up = glm::normalize(glm::cross(_right, _front));

    _view = glm::lookAt(_position, _position + _front, _up);
}
