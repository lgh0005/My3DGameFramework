#include "EnginePch.h"
#include "CapsuleCollider.h"
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

DECLARE_DEFAULTS_IMPL(CapsuleCollider)

CapsuleColliderUPtr CapsuleCollider::Create(float radius, float height)
{
	auto collider = CapsuleColliderUPtr(new CapsuleCollider());
	if (collider->Init(radius, height)) return collider;
	return nullptr;
}

bool CapsuleCollider::Init(float radius, float height)
{
	m_radius = radius;
	m_height = height;
	m_shape = CreateShape();
	if (!m_shape) return false;
	return true;
}

JPH::ShapeRefC CapsuleCollider::CreateRawShape()
{
	// 1. 안전한 최소값 보정
	float safeRadius = glm::max<float>(m_radius, 0.001f);
	float safeHeight = glm::max<float>(m_height, 0.001f);

	// 2. Jolt 파라미터 변환
	// Jolt: CapsuleShape(float inHalfHeightOfCylinder, float inRadius)
	// 입력값: 전체 높이(Tip-to-Tip)
	// 변환식: 실린더 절반 높이 = (전체 높이 / 2) - 반지름
	float halfHeightOfCylinder = (safeHeight * 0.5f) - safeRadius;

	// 3. 예외 처리: 높이가 지름보다 작은 경우
	if (halfHeightOfCylinder < 0.0f) halfHeightOfCylinder = 0.0f;

	// 4. Shape 생성 (기본적으로 Y축 정렬됨)
	return new JPH::CapsuleShape(halfHeightOfCylinder, safeRadius);
}

void CapsuleCollider::SetRadius(float radius)
{
	if (m_radius == radius) return;
	m_radius = radius;
	m_shape = CreateShape();
}

void CapsuleCollider::SetHeight(float height)
{
	if (m_height == height) return;
	m_height = height;
	m_shape = CreateShape();
}