#pragma once
#include<DxLib.h>

class GameObject abstract
{
public:
	GameObject() {};
	virtual ~GameObject() {};
	virtual void Init() abstract;
	virtual void Update() abstract;
	virtual void Draw() const abstract;

	VECTOR GetPos() { return _pos; }

	enum class CollisionType
	{
		Sphere,
		Poly,
	};

	void SetCollision(bool isCollidable, CollisionType collisionType, float radius = 0.0f, int polyH = -1)
	{
		_isCollidable = isCollidable;
		_collisionType = collisionType;
		_radius = radius;
		_polyH = polyH;
	}

	bool IsCollidable() const { return _isCollidable; }
	CollisionType GetCollisionType() const { return _collisionType; }
	float GetRadius() const { return _radius; }
	int GetPolyH() const { return _polyH; }
	VECTOR GetPos() const { return _pos; }
	virtual void SetPos(VECTOR pos) { _pos = pos; }
	VECTOR GetVec() const { return _vec; }
	void SetVec(VECTOR vec) { _vec = vec; }
	bool IsGrounded() { return _isGrounded; }
	void SetGrounded(bool isGrounded) { _isGrounded = isGrounded; }
protected:
	VECTOR _pos{ 0.0f, 0.0f, 0.0f };
	VECTOR _vec{ 0.0f, 0.0f, 0.0f };
	bool _isCollidable = false;
	CollisionType _collisionType;
	float _radius = 0.0f;
	int _polyH = -1;
	bool _isGrounded = true;
};

