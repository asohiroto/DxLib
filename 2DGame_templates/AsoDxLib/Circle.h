#pragma once
#include "Vec2.h"

class Circle
{
  public:
    // 中心座標
    Vec2 centerPos;
    // 半径
    float radius;

    Circle() : centerPos(0, 0), radius(0)
    {
    }

    Circle(Vec2 pos, float rad) : centerPos(pos), radius(rad)
    {
    }
};