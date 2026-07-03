#pragma once

/// <summary>
/// 当たり判定の形によって処理が変わるので
/// お互いがどの形の当たり判定をしているのかを
/// 先にチェックする際に使う
/// </summary>
enum class Shape
{
	None,
	Sphere,
	Capsule,
	Polygon,
	Torus,
};

/// <summary>
/// 当たり判定の形に関する基底クラス
/// </summary>
class ColliderBase abstract
{
public:
	ColliderBase(Shape shape);
	virtual ~ColliderBase() {};
	//当たり判定の形
	Shape m_shape;
};

