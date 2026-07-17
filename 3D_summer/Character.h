#pragma once
#include<DxLib.h>
#include<memory>

class Input;

class Character
{
public:
	enum class CollisionType
	{
		Sphere,
		Polygon,
		Capsule
	};

	enum class PlayerState
	{
		Idle,
		Move,
		Dodge,
		WAttack,
		SAttack
	};

public:
	Character() {};
	virtual ~Character() {};
	virtual void Init() {};
	virtual void Update(float cameraAngle, std::shared_ptr<Input> pInput) {};
	virtual void Draw() {};
	// _posのゲッター
	VECTOR GetPos() const { return _pos; }
	// _posのセッター
	void SetPos(VECTOR pos) { _pos = pos; }
	// コリジョン情報のセッター
	void SetCollision(bool isCollidable, CollisionType collisionType, float radius, int polygonH)
	{
		_isCollidable = isCollidable;
		_collisionType = collisionType;
		_radius = radius;
		_polygonH = polygonH;
	}
	// コリジョン可能か
	bool IsCollidable() const { return _isCollidable; }
	// いずれのコリジョンタイプか
	CollisionType GetCollType() const { return _collisionType; }
	// 半径のゲッター
	float GetRadius() const { return _radius; }
	// ポリゴンハンドルの取得
	int GetPolH() const { return _polygonH; }

protected:
	// モデルのハンドル
	int _modelH = -1;
	// 位置
	VECTOR _pos = VGet(0.0f, 0.0f, 0.0f);
	// コリジョン可能か
	bool _isCollidable = false;
	// コリジョン判定の種類
	CollisionType _collisionType = CollisionType::Sphere;
	// 半径
	float _radius = 0.0f;
	// ポリゴンのハンドル
	int _polygonH = -1;
};

