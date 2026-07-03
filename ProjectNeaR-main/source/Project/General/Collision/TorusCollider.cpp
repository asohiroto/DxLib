#include "TorusCollider.h"

TorusCollider::TorusCollider():
	ColliderBase(Shape::Torus),
	m_range(0.0f),
	m_radius(0.0f)
{
}
