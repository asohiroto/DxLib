#include "Collidable.h"
#include "Rigidbody.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "PolygonCollider.h"
#include "TorusCollider.h"
#include "Physics.h"
Collidable::Collidable(Shape shape):
	m_isThrough(false),
	m_isTrigger(false),
	m_collState(CollisionState::Normal),
	m_priority(Priority::None),
	m_tag(GameTag::None),
	m_isFloor(false),
	m_isWall(false)
{
	CreateCollider(shape);
	m_rb = std::make_shared<Rigidbody>();
}

void Collidable::Init()
{
	//PhysicsÇ…ìoò^
	Physics::GetInstance().Entry(shared_from_this());
}

void Collidable::End()
{
	//PhysicsÇ©ÇÁìoò^âèú
	Physics::GetInstance().Exit(shared_from_this());
}

void Collidable::SetTimeScale(float scale)
{
	if (m_rb)
	{
		m_rb->SetMyTimeScale(scale);
	}
}

void Collidable::CreateCollider(Shape shape)
{
	switch (shape)
	{
	case Shape::Sphere:
		m_collisionData = std::make_shared<SphereCollider>();
		break;
	case Shape::Capsule:
		m_collisionData = std::make_shared<CapsuleCollider>();
		break;
	case Shape::Polygon:
		m_collisionData = std::make_shared<PolygonCollider>();
		break;
	case Shape::Torus:
		m_collisionData = std::make_shared<TorusCollider>();
		break;
	default:
		m_collisionData = nullptr;
		break;
	}
}

void Collidable::AllSetting(Priority priority, GameTag tag, bool isTrough, bool isTrigger, bool isGravity)
{
	m_priority = priority;			//óDêÊìx
	m_tag = tag;					//É^ÉO
	m_isThrough = isTrough;			//ìñÇΩÇËîªíËÇñ≥éãÇ∑ÇÈÇ©
	m_isTrigger = isTrigger;		//ÉgÉäÉKÅ[îªíËÇÇ∑ÇÈÇ©
	m_rb->m_isGravity = isGravity;	//èdóÕÇéÛÇØÇÈÇ©
}
