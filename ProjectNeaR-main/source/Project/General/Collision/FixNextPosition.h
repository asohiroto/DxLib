#pragma once
#include <memory>
#include <DxLib.h>
#include <vector>
#include "../Math/MyMath.h"
namespace
{
	//ポリゴンの当たり判定の配列の最大数
	constexpr int kMaxHitPolygon = 2048;
}
class Collidable;
class Physics;
class FixNextPosition
{
public:
	FixNextPosition();
	~FixNextPosition();

	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="collA"></param>
	/// <param name="collB"></param>
	void FixNextPos(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB);
private:
	/// <summary>
	/// 球と球の衝突処理
	/// </summary>
	/// <param name="collA">球</param>
	/// <param name="collB">球</param>
	void FixNextPosSS(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB);
	/// <summary>
	/// 球とポリゴンの衝突処理
	/// </summary>
	/// <param name="collA">球</param>
	/// <param name="collB">ポリゴン</param>
	void FixNextPosSP(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB);
	/// <summary>
	/// カプセルとカプセルの衝突処理
	/// </summary>
	/// <param name="collA">カプセル</param>
	/// <param name="collB">カプセル</param>
	void FixNextPosCC(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB);
	/// <summary>
	/// カプセルと球
	/// </summary>
	/// <param name="collA">カプセル</param>
	/// <param name="collB">球</param>
	void FixNextPosCS(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB);
	/// <summary>
	/// カプセルとポリゴンの衝突処理
	/// </summary>
	/// <param name="collA">カプセル</param>
	/// <param name="collB">ポリゴン</param>
	void FixNextPosCP(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB);

	/// <summary>
	/// 床ポリゴンと壁ポリゴンに分ける
	/// </summary>
	void AnalyzeWallAndFloor(MV1_COLL_RESULT_POLY_DIM hitDim, const Vector3& oldPos);
	/// <summary>
	/// 球とポリゴンの押し戻しベクトルを返す
	/// </summary>
	Vector3 OverlapVecSphereAndPoly(const Vector3& nextPos, std::vector<MV1_COLL_RESULT_POLY>&  dim, float shortDis);
	/// <summary>
	/// 壁と当たった時の処理 カプセル
	/// </summary>
	Vector3 HitWallCP(const Vector3& headPos, const Vector3& legPos, float shortDis);
	/// <summary>
	/// 床の高さに合わせる カプセル
	/// </summary>
	bool HitFloorCP(const std::shared_ptr<Collidable> other, const Vector3& legPos, const Vector3& headPos, float shortDis);
	/// <summary>
	/// 天井に当たった時の処理 カプセル
	/// </summary>
	void HitRoofCP(const std::shared_ptr<Collidable> other, const Vector3& headPos, float shortDis);
private:
	std::vector<MV1_COLL_RESULT_POLY> m_wall;
	std::vector<MV1_COLL_RESULT_POLY> m_floorAndRoof;
};

