#pragma once
#include "ColliderBase.h"
#include "../Math/MyMath.h"
class CapsuleCollider :
    public ColliderBase
{
public:
    CapsuleCollider();
    virtual ~CapsuleCollider() {};
    //自分の座標とm_endPosの2点で構成されたカプセル
    Position3 GetEndPos() const { return m_endPos; };
    void SetEndPos(Position3 pos) { m_endPos = pos; };
    //次の座標
    Position3 GetNextEndPos(Vector3 vec) const { return m_endPos + vec; };
    void SetNextEndPos(Vector3 addVec) { m_endPos += addVec; };
    //2点の球の半径
    float GetRadius() const { return m_radius; };
    void SetRadius(float radius) { m_radius = radius; };
    //当たった物体と近い座標
    Vector3 GetNearPos() const { return m_hitNearPos; };
    void SetNearPos(Position3 hitNearPos) { m_hitNearPos = hitNearPos; };
    //最短距離
    float GetShortDis() { return m_shortDis; }
    void SetShortDis(float shortDis) { m_shortDis = shortDis; };
private:
    //自分の座標とm_endPosの2点で構成されたカプセル
    Position3 m_endPos;
    //2点の球の半径
    float m_radius;
    //当たった物体と近い座標
    Vector3 m_hitNearPos;
    //最短距離
    float m_shortDis;
};

