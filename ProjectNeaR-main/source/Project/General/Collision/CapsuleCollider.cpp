#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider():
	ColliderBase(Shape::Capsule),
	m_endPos{0.0f,0.0f,0.0f},
	m_radius(0.0f),
	m_shortDis(0.0f),
	m_hitNearPos{ 0.0f, 0.0f, 0.0f }
{
}