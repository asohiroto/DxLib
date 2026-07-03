#pragma once
#include "ColliderBase.h"
#include "../Math/MyMath.h"
class TorusCollider :
    public ColliderBase
{
public:
	TorusCollider();
    virtual ~TorusCollider() {};
    //”ÍˆÍæ“¾
	float GetRange() const { return m_range; }
	//”ÍˆÍİ’è
	void  SetRange(float range)  { m_range = range; }
	//”¼Œaæ“¾
	float GetRadius() const { return m_radius; }
	//”¼Œaİ’è
	void  SetRadius(float radius) { m_radius = radius; }
private:
    //”ÍˆÍ
	float m_range;
    //”¼Œa
	float m_radius;
};

