#include "CollisionChecker.h"
#include "../Math/MyMath.h"
#include <DxLib.h>
#include "Collidable.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "PolygonCollider.h"
#include "TorusCollider.h"
#include "Rigidbody.h"
#include "../../Main/Application.h"

CollisionChecker::CollisionChecker()
{
}


bool CollisionChecker::IsCollide(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//コライダーの形状
	auto shapeA = collA->m_collisionData->m_shape;
	auto shapeB = collB->m_collisionData->m_shape;

	//当たったかの結果
	bool isHit = false;
	//球と
	if (shapeA == Shape::Sphere)
	{
		//球
		if (shapeB == Shape::Sphere)
		{
			isHit = CheckCollSS(collA, collB);
		}
		//カプセル
		else if (shapeB == Shape::Capsule)
		{
			isHit = CheckCollCS(collB, collA);
		}
		//ポリゴン
		else if (shapeB == Shape::Polygon)
		{
			isHit = CheckCollSP(collA, collB);
		}
		//ドーナツ
		else if (shapeB == Shape::Torus)
		{
			isHit = CheckCollSD(collA, collB);
		}
	}
	//カプセルと
	else if (shapeA == Shape::Capsule)
	{
		//球
		if (shapeB == Shape::Sphere)
		{
			isHit = CheckCollCS(collA, collB);
		}
		//カプセル
		else if (shapeB == Shape::Capsule)
		{
			isHit = CheckCollCC(collA, collB);
		}
		//ポリゴン
		else if (shapeB == Shape::Polygon)
		{
			isHit = CheckCollCP(collA, collB);
		}
		//ドーナツ
		else if (shapeB == Shape::Torus)
		{
			isHit = CheckCollCD(collA, collB);
		}
	}
	//ポリゴンと
	else if (shapeA == Shape::Polygon)
	{
		//球
		if (shapeB == Shape::Sphere)
		{
			isHit = CheckCollSP(collB, collA);
		}
		//カプセル
		else if (shapeB == Shape::Capsule)
		{
			isHit = CheckCollCP(collB, collA);
		}
	}
	return isHit;
}

bool CollisionChecker::CheckCollSS(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{

	//コリジョンデータの取得
	auto collDataA = std::dynamic_pointer_cast<SphereCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<SphereCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;

	//Aのコライダーの情報
	Vector3 posA = rbA->GetNextPos();
	float radiusA = collDataA->GetRadius();
	//Bのコライダーの情報
	Vector3 posB = rbB->GetNextPos();
	float radiusB = collDataB->GetRadius();

	Vector3 aToB = posB - posA;

	//２物体間の長さが半径の合計より大きいなら当たっていない
	if (aToB.Magnitude() >= radiusA + radiusB)
	{
		return false;
	}

	//ここまで来たら当たっている
	return true;
}

bool CollisionChecker::CheckCollCS(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//球からカプセルに垂線を引いて球とカプセルの最短距離を求める

	//コライダーデータを取得
	auto collDataA = std::dynamic_pointer_cast<CapsuleCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<SphereCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;

	//カプセルのそれぞれの座標
	Vector3 cPosA = rbA->GetNextPos();
	Vector3 cPosB = collDataA->GetNextEndPos(rbA->GetVecWithTS());
	//球の座標
	Vector3 sPos = rbB->GetNextPos();
	//最短距離
	float shortDis = collDataA->GetRadius() + collDataB->GetRadius();

	//カプセルの座標Aから球へのベクトル
	Vector3 AtoS = sPos - cPosA;
	//カプセルの座標AからBへのベクトル
	Vector3 AtoB = cPosB - cPosA;

	//内積
	float dot = AtoS.Dot(AtoB);
	//ABの長さの二乗で割る
	float t = dot / AtoB.SqMagnitude();
	//クランプ
	t = MathSub::ClampFloat(t, 0.0f, 1.0f);
	//最短座標を出す
	Vector3 minPos = cPosA + (AtoB * t);
	
	float distance = (sPos - minPos).Magnitude();
	//最短より大きいなら
	if (distance >= shortDis)
	{
		return false;
	}
	
	//衝突判定で使うので一番近い座標を覚えておく
	collDataA->SetNearPos(minPos);
	//ここまでくれば当たっている
	return true;
}

//衝突判定のために最近接点の計算もしてる
bool CollisionChecker::CheckCollCC(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	// どちらかのカプセルの一つの座標から片方のカプセルに垂線を引いて
	//　お互いのカプセルに最も近い座標をそれぞれ出す

	//コライダーデータを取得
	auto collDataA = std::dynamic_pointer_cast<CapsuleCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<CapsuleCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;

	//カプセルのそれぞれの座標
	//カプセルA
	Vector3 capAStartPos = rbA->GetNextPos();
	Vector3 capAEndPos = collDataA->GetNextEndPos(rbA->GetVecWithTS());
	float capARadius = collDataA->GetRadius();
	//カプセルB
	Vector3 capBStartPos = rbB->GetNextPos();
	Vector3 capBEndPos = collDataB->GetNextEndPos(rbB->GetVecWithTS());
	float capBRadius = collDataB->GetRadius();
	
	//平行かどうか確認する
	auto capADir = capAEndPos - capAStartPos;//線分1
	auto capBDir = capBStartPos - capBEndPos;//線分2
	//正規化して内席から平行かチェック
	auto cross = capADir.Cross(capBDir);
	//平行なら
	if (cross.SqMagnitude() <= 0.0f)
	{
		return ParallelCC(collA, collB);
	}

	//最短距離
	float shortDis = capARadius + capBRadius;
	//今の最短距離
	float nowShortDis = 10000.0f;//始点同士を最短にしておく

	for (int i = 0; i < 2; ++i)
	{
		//線分のそれぞれの座標
		Vector3 lineStart;
		Vector3 lineEnd;
		//最初にカプセルBに対してカプセルAのそれぞれの点から最短の座標を出す
		if (i <= 0)
		{
			//線分CD
			lineStart = capBStartPos;
			lineEnd = capBEndPos;
		}
		//次ににカプセルAに対してカプセルBのそれぞれの点から最短の座標を出す
		else
		{
			//線分AB
			lineStart = capAStartPos;
			lineEnd = capAEndPos;
		}

		for (int j = 0; j < 2; ++j)
		{
			//確認する座標
			Vector3 checkPoint;
			//Aから線分に下ろす
			if (i == 0 && j == 0)checkPoint = capAStartPos;
			//Bから線分に下ろす
			if (i == 0 && j == 1)checkPoint = capAEndPos;
			//Cから線分に下ろす
			if (i == 1 && j == 0)checkPoint = capBStartPos;
			//Dから線分に下ろす
			if (i == 1 && j == 1)checkPoint = capBEndPos;

			//あとは球とカプセルの時と同じ
			//Aから確認する座標へのベクトル
			Vector3 lineAtoP = checkPoint - lineStart;
			//線分ABの単位ベクトル
			Vector3 lineAB = lineEnd - lineStart;

			//それぞれのベクトルから内積をだして球から垂線を下した位置を求める
			float dot = lineAtoP.Dot(lineAB);
			//ABの長さの二乗で割る
			float t = dot / lineAB.SqMagnitude();
			//クランプ
			t = MathSub::ClampFloat(t, 0.0f, 1.0f);

			//最短座標を出す
			Vector3 minPos = lineStart + (lineAB * t);
			
			//最短距離
			float distance = (checkPoint - minPos).Magnitude();
			
			//初回または前回の最短距離より小さいなら現在の最短距離とする
			if (distance < nowShortDis)
			{
				nowShortDis = distance;

				//最短座標を記録
				if (i == 0)
				{
					//衝突判定で使うので一番近い座標を覚えておく
					collDataB->SetNearPos(minPos);
				}
				else
				{
					//衝突判定で使うので一番近い座標を覚えておく
					collDataA->SetNearPos(minPos);
				}
			}
		}
	}

	//現在の最短距離が変形の合計より大きいなら当たっていない
	if (nowShortDis >= shortDis)
	{
		return false;
	}


	//ここまでくれば当たっている
	return true;
}

bool CollisionChecker::CheckCollCCVerDxLib(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//コライダーデータを取得
	auto collDataA = std::dynamic_pointer_cast<CapsuleCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<CapsuleCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;

	//カプセルのそれぞれの座標
	//カプセルA
	Vector3 capAStartPos = rbA->GetNextPos();
	Vector3 capAEndPos = collDataA->GetNextEndPos(rbA->GetVecWithTS());
	float capARadius = collDataA->GetRadius();
	//カプセルB
	Vector3 capBStartPos = rbB->GetNextPos();
	Vector3 capBEndPos = collDataB->GetNextEndPos(rbB->GetVecWithTS());
	float capBRadius = collDataB->GetRadius();

	return HitCheck_Capsule_Capsule(capAStartPos.ToDxLibVector(), capAEndPos.ToDxLibVector(), capARadius,
		capBStartPos.ToDxLibVector(), capBEndPos.ToDxLibVector(), capBRadius);
}

bool CollisionChecker::CheckCollSP(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//コライダーデータの取得
	auto collDataA = std::dynamic_pointer_cast<SphereCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<PolygonCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;

	//当たってるポリゴンの数
	auto hitDim = MV1CollCheck_Sphere(
		collDataB->GetModelHandle(),
		-1,
		rbA->GetNextPos().ToDxLibVector(),
		collDataA->GetRadius());
	//当たっていないなら
	if (hitDim.HitNum <= 0)
	{
		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hitDim);
		
		//連続的衝突判定（Continuous Collision Detection）
		auto lineHitDim = MV1CollCheck_Line(
			collDataB->GetModelHandle(),
			-1,
			rbA->GetPos().ToDxLibVector(),
			rbA->GetNextPos().ToDxLibVector()
		);

		//当たったなら
		if (lineHitDim.HitFlag)
		{
			//CCD判定をした
			collDataB->SetIsCCD(true);

			//当たり判定に使うので保存
			collDataB->SetLineHit(lineHitDim);

			return true;
		}
		else
		{
			return false;
		}
	}

	if (collA->m_isTrigger)
	{
		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hitDim);
	}
	else
	{
		//当たり判定に使うので保存
		collDataB->SetHitDim(hitDim);
	}

	return true;
}

bool CollisionChecker::CheckCollCP(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//コライダーデータの取得
	auto collDataA = std::dynamic_pointer_cast<CapsuleCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<PolygonCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;

	//当たってるポリゴンの数
	auto hitDim = MV1CollCheck_Capsule(
		collDataB->GetModelHandle(),
		-1,
		rbA->GetNextPos().ToDxLibVector(),
		collDataA->GetNextEndPos(rbA->GetVecWithTS()).ToDxLibVector(),
		collDataA->GetRadius(),
		-1);

	//当たっていないなら
	if (hitDim.HitNum <= 0)
	{
		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hitDim);

		//連続的衝突判定（Continuous Collision Detection）
		auto startLineHitDim = MV1CollCheck_Line(
			collDataB->GetModelHandle(),
			-1,
			rbA->GetPos().ToDxLibVector(),
			rbA->GetNextPos().ToDxLibVector()
		);
		//当たったのであれば
		if (startLineHitDim.HitFlag)
		{
			//CCD判定をした
			collDataB->SetIsCCD(true);

			//当たり判定に使うので保存
			collDataB->SetLineHit(startLineHitDim);

			return true;
		}
		else
		{
			auto endLineHitDim = MV1CollCheck_Line(
				collDataB->GetModelHandle(),
				-1,
				collDataA->GetEndPos().ToDxLibVector(),
				collDataA->GetNextEndPos(rbA->GetVecWithTS()).ToDxLibVector()
			);

			if (endLineHitDim.HitFlag)
			{
				//CCD判定をした
				collDataB->SetIsCCD(true);

				//当たり判定に使うので保存
				collDataB->SetLineHit(endLineHitDim);

				return true;
			}
			else
			{
				//当たってない
				return false;
			}
		}
	}
	if (collA->m_isTrigger)
	{
		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		MV1CollResultPolyDimTerminate(hitDim);
	}
	else
	{
		//当たり判定に使うので保存
		collDataB->SetHitDim(hitDim);
	}

	return true;
}

bool CollisionChecker::CheckCollSD(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//コリジョンデータの取得
	auto collDataA = std::dynamic_pointer_cast<SphereCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<TorusCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;
	//Aのコライダーの情報
	Vector3 posA = rbA->GetNextPos();
	float radiusA = collDataA->GetRadius();
	//Bのコライダーの情報
	Vector3 posB = rbB->GetNextPos();
	float radiusB = collDataB->GetRadius();
	float rangeB = collDataB->GetRange();
	
	//XZ平面での距離
	float dx = posB.x - posA.x;
	float dz = posB.z - posA.z;
	float disXZ = sqrtf(dx * dx + dz * dz);

	//ドーナツの断面中心までのXZ平面での距離の差
	float disFromRingCenterXZ = disXZ - rangeB;
	//Y方向の差
	float dy = posB.y - posA.y;
	//ドーナツ断面中心から球の中心までの距離
	float tubeDis = sqrtf(disFromRingCenterXZ * disFromRingCenterXZ + dy * dy);

	return (tubeDis <= (radiusA + radiusB));
}

bool CollisionChecker::CheckCollCD(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//コライダーデータを取得
	auto collDataA = std::dynamic_pointer_cast<CapsuleCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<TorusCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;
	//Aのコライダーの情報
	Vector3 cPosA = rbA->GetNextPos();
	Vector3 cPosB = collDataA->GetNextEndPos(rbA->GetVecWithTS());
	float radiusA = collDataA->GetRadius();
	//Bのコライダーの情報
	Vector3 posB = rbB->GetNextPos();
	float radiusB = collDataB->GetRadius();
	float rangeB = collDataB->GetRange();

	//カプセル中心からドーナツの中心までの距離を求める
	Vector3 capsuleCenterXZ = { (cPosA.x + cPosB.x) * 0.5f, 0, (cPosA.z + cPosB.z) * 0.5f };
	Vector3 torusCenterXZ = { posB.x, 0, posB.z };
	Vector3 toCapsuleXZ = capsuleCenterXZ - torusCenterXZ;
	float distXZ = toCapsuleXZ.Magnitude();

	//距離0のときは任意にX方向に設定（ゼロ割り対策）
	Vector3 dirXZ = Vector3{ 1, 0, 0 };
	if(distXZ > 0.0f)
	{
		dirXZ = toCapsuleXZ.Normalize();
	}

	//ドーナツの断面の中心候補（XZ平面上）
	Vector3 ringPoint = posB;
	ringPoint.x += dirXZ.x * rangeB;
	ringPoint.z += dirXZ.z * rangeB;

	//その点とカプセルとの最短距離
	Vector3 AB = cPosB - cPosA;
	Vector3 AP = ringPoint - cPosA;

	float t = MathSub::ClampFloat(AP.Dot(AB) / AB.Dot(AB), 0.0f, 1.0f);
	Vector3 closest = cPosA + AB * t;
	float dis = (ringPoint - closest).Magnitude();

	//管とカプセルが交差しているか
	return dis <= (radiusA + radiusB);
}

bool CollisionChecker::ParallelCC(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//コライダーデータを取得
	auto collDataA = std::dynamic_pointer_cast<CapsuleCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<CapsuleCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;

	//カプセルA
	Vector3 cPosA = rbA->GetNextPos();
	Vector3 cPosB = collDataA->GetNextEndPos(rbA->GetVecWithTS());
	//カプセルB
	Vector3 cPosC = rbB->GetNextPos();
	Vector3 cPosD = collDataB->GetNextEndPos(rbB->GetVecWithTS());
	//最短距離
	float shortDis = collDataA->GetRadius() + collDataB->GetRadius();
	
	//各距離をチェック
	Vector3 ac = cPosC - cPosA;
	Vector3 ad = cPosD - cPosA;
	Vector3 bc = cPosC - cPosB;
	Vector3 bd = cPosD - cPosB;
	//最短距離を出す
	float dis = ac.Magnitude();
	//一度入れておく
	Vector3 nearPosA = cPosA;
	Vector3 nearPosB = cPosC;
	//短いなら
	if (dis > ad.Magnitude())
	{
		dis = ad.Magnitude();
		nearPosA = cPosA;
		nearPosB = cPosD;
	}
	if (dis > bc.Magnitude())
	{
		dis = bc.Magnitude();
		nearPosA = cPosB;
		nearPosB = cPosC;
	}
	if (dis > bd.Magnitude())
	{
		dis = bd.Magnitude();
		nearPosA = cPosB;
		nearPosB = cPosD;
	}
	collDataA->SetNearPos(nearPosA);
	collDataB->SetNearPos(nearPosB);

	//最短距離より大きいなら当たっていない
	if (dis >= shortDis)
	{
		return false;
	}

	//ここまで来たら当たってる
	return true;
}