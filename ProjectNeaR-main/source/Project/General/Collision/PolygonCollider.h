#pragma once
#include "ColliderBase.h"
#include "../Math/MyMath.h"
#include <DxLib.h>

//モデルのポリゴンが当たり判定
class PolygonCollider :
    public ColliderBase
{
public:
	PolygonCollider();
	virtual ~PolygonCollider();
	MV1_COLL_RESULT_POLY_DIM& GetHitDim() { return m_hitDim; }
	void SetHitDim(MV1_COLL_RESULT_POLY_DIM& dim){ m_hitDim = dim; }
	MV1_COLL_RESULT_POLY GetLineHit()const{ return m_lineHit; }
	void SetLineHit(MV1_COLL_RESULT_POLY lineHit){ m_lineHit = lineHit; }
    //壁と床の近い座標
	Vector3 GetNearWallHitPos() const { return m_nearWallHitPos; }
	Vector3 GetNearFloorHitPos() const { return m_nearFloorHitPos; }
    //モデル
	int GetModelHandle() const{ return m_modelHandle; }
	void SetModelHandle(int modelHandle) { m_modelHandle = modelHandle; }
    //CCDをしたか
    bool IsCCD()const { return m_isCCD; };
    void SetIsCCD(bool isCCD) { m_isCCD = isCCD; };
private:
    //当たり判定をするモデル
    int m_modelHandle;
    //当たったポリゴンの情報
    MV1_COLL_RESULT_POLY_DIM m_hitDim;
    //線分とポリゴンの当たり判定情報
    MV1_COLL_RESULT_POLY m_lineHit;
    //当たった際の最も近い壁ポリゴンの座標
    Vector3 m_nearWallHitPos;
    //当たった際の最も近い床ポリゴンの座標
    Vector3 m_nearFloorHitPos;
    //連続的衝突判定（Continuous Collision Detection）をしたか
    bool m_isCCD;
};

