#include "PolygonCollider.h"

PolygonCollider::PolygonCollider():
	ColliderBase(Shape::Polygon),
	m_modelHandle(-1),
	m_nearWallHitPos(),
	m_nearFloorHitPos(),
	m_hitDim(),
    m_lineHit(),
    m_isCCD(false)
{
}

PolygonCollider::~PolygonCollider()
{
  
}
