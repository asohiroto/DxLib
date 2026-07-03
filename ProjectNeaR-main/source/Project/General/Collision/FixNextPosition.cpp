#include "FixNextPosition.h"
#include "Collidable.h"
#include "Rigidbody.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "PolygonCollider.h"
#include <vector>

namespace
{
	//押し戻しの値に足して密着するのを防ぐ
	constexpr float kOverlapGap = 1.0f;
	constexpr float kCheckUnder = -800.0f;
	constexpr float kCheckTop = 800.0f;
	constexpr float kWallThreshold = 0.5f;
}

FixNextPosition::FixNextPosition() :
	m_wall(),
	m_floorAndRoof()
{
}

FixNextPosition::~FixNextPosition()
{
}

void FixNextPosition::FixNextPos(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//初期化
	m_wall.clear();
	m_floorAndRoof.clear();

	//衝突しているオブジェクトの形状を取得
	auto collAShape = collA->m_collisionData->m_shape;
	auto collBShape = collB->m_collisionData->m_shape;
	//球と
	if (collAShape == Shape::Sphere)
	{
		//球
		if (collBShape == Shape::Sphere)
		{
			//ベクトルを補正する
			FixNextPosSS(collA, collB);
		}
		//カプセル
		else if (collBShape == Shape::Capsule)
		{
			//ベクトルを補正する
			FixNextPosCS(collB, collA);
		}
		//ポリゴン
		else if (collBShape == Shape::Polygon)
		{
			//ベクトルを補正する
			FixNextPosSP(collA, collB);
		}
	}
	//カプセルと
	else if (collAShape == Shape::Capsule)
	{
		//球
		if (collBShape == Shape::Sphere)
		{
			//ベクトルを補正する
			FixNextPosCS(collA, collB);
		}
		//カプセル
		else if (collBShape == Shape::Capsule)
		{
			//ベクトルを補正する
			FixNextPosCC(collA, collB);
		}
		//ポリゴン
		else if (collBShape == Shape::Polygon)
		{
			//ベクトルを補正する
			FixNextPosCP(collA, collB);
		}
	}
	//ポリゴンと
	else if (collAShape == Shape::Polygon)
	{
		//球
		if (collBShape == Shape::Sphere)
		{
			//ベクトルを補正する
			FixNextPosSP(collB, collA);
		}
		//カプセル
		else if (collBShape == Shape::Capsule)
		{
			//ベクトルを補正する
			FixNextPosCP(collB, collA);
		}
	}
}

void FixNextPosition::FixNextPosSS(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//優先度
	auto priorityA = collA->m_priority;
	auto priorityB = collB->m_priority;
	//お互い動かないオブジェクトなら衝突しない
	if (priorityA == Priority::Static &&
		priorityB == Priority::Static)return;

	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;

	//AからBへのベクトル
	Vector3 aToB = rbB->GetNextPos() - rbA->GetNextPos();
	//最短距離
	float shortDis = std::dynamic_pointer_cast<SphereCollider> (collA->m_collisionData)->GetRadius() + std::dynamic_pointer_cast<SphereCollider> (collB->m_collisionData)->GetRadius();
	//どのくらい重ねっているか
	float overlap = shortDis - aToB.Magnitude();
	overlap = MathSub::ClampFloat(overlap, 0, shortDis);
	overlap += kOverlapGap;

	//優先度から動かすほうを決める
	if (priorityA > priorityB)
	{
		rbB->AddVec(aToB.Normalize() * overlap);
	}
	else if (priorityA < priorityB)
	{
		rbA->AddVec(aToB.Normalize() * -overlap);
	}
	else
	{
		rbA->AddVec(aToB.Normalize() * -overlap / 2.0f);
		rbB->AddVec(aToB.Normalize() * overlap / 2.0f);
	}
}

void FixNextPosition::FixNextPosSP(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	
	//コリジョンデータ
	auto collDataA = std::dynamic_pointer_cast<SphereCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<PolygonCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;
	
	//当たったポリゴンの情報
	auto& hitDim = collDataB->GetHitDim();

	//お互い動かないオブジェクトなら衝突しない(ポリゴンはスタティックなので片方がスタティックなら)
	if (collA->m_priority == Priority::Static)
	{
		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		DxLib::MV1CollResultPolyDimTerminate(hitDim);
		return;
	}

	//球の座標
	Position3 nextPos = rbA->GetNextPos();//移動後

	//床ポリゴンと壁ポリゴンに分ける
	AnalyzeWallAndFloor(hitDim, nextPos);

	//床か天井に当たったか
	bool isFloorAndRoof = !m_floorAndRoof.empty();
	//壁に当たったか
	bool isWall = !m_wall.empty();

	//床と当たったなら
	if (isFloorAndRoof)
	{
		//補正するベクトルを返す
		Vector3 overlapVec = OverlapVecSphereAndPoly(nextPos, m_floorAndRoof, collDataA->GetRadius());
	
		//ポリゴンは固定(static)なので球のみ動かす
		rbA->AddVec(overlapVec);
		//修正方向が上向きなら床
		if (overlapVec.y > 0)
		{
			//床に当たっているので
			collA->SetIsFloor(true);
		}
	}
	//壁と当たっているなら
	if(isWall)
	{
		//壁に当たっているので
		collA->SetIsWall(true);

		//補正するベクトルを返す
		Vector3 overlapVec = OverlapVecSphereAndPoly(nextPos, m_wall, collDataA->GetRadius());
		
		//位置を補正
		Vector3 newPos = rbA->GetNextPos() + overlapVec;
		rbA->SetPos(newPos);

		//横方向の速度をリセット（めり込み反動防止）
		rbA->SetMoveVec(Vector3::Zero());
	}

	// 検出したプレイヤーの周囲のポリゴン情報を開放する
	DxLib::MV1CollResultPolyDimTerminate(hitDim);
}

void FixNextPosition::FixNextPosCC(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//優先度
	auto priorityA = collA->m_priority;
	auto priorityB = collB->m_priority;
	//お互い動かないオブジェクトなら衝突しない
	if (priorityA == Priority::Static &&
		priorityB == Priority::Static)return;

	//カプセルの押し戻しはそれぞれの当たったポイントから計算します

	//コライダーデータ
	auto collDataA = std::dynamic_pointer_cast<CapsuleCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<CapsuleCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;

	//AからBへのベクトル
	Vector3 aToB = collDataB->GetNearPos() - collDataA->GetNearPos();
	//最短距離
	float shortDis = collDataA->GetRadius() + collDataB->GetRadius();
	//どのくらい重ねっているか
	float overlap = shortDis - aToB.Magnitude();
	overlap = MathSub::ClampFloat(overlap, 0, shortDis);
	overlap += kOverlapGap;

	//横方向にだけ動かしたいので
	aToB.y = 0.0f;

	//動かす物体とそうじゃない物体とで処理を分ける
	if (priorityA > priorityB)
	{
		rbB->AddVec(aToB.Normalize() * overlap);
	}
	else if (priorityA < priorityB)
	{
		rbA->AddVec(aToB.Normalize() * -overlap);
	}
	else
	{
		rbA->AddVec(aToB.Normalize() * -overlap / 2.0f);
		rbB->AddVec(aToB.Normalize() * overlap / 2.0f);
	}
}

void FixNextPosition::FixNextPosCS(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//優先度
	auto priorityA = collA->m_priority;
	auto priorityB = collB->m_priority;
	//お互い動かないオブジェクトなら衝突しない
	if (priorityA == Priority::Static &&
		priorityB == Priority::Static)return;
	//コライダーデータ
	auto collDataA = std::dynamic_pointer_cast<CapsuleCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<SphereCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;

	//AからBへのベクトル
	Vector3 aToB = rbB->GetNextPos() - collDataA->GetNearPos();

	//最短距離
	float shortDis = collDataB->GetRadius() + collDataA->GetRadius();
	//どのくらい重ねっているか
	float overlap = shortDis - aToB.Magnitude();
	overlap = MathSub::ClampFloat(overlap, 0, shortDis);
	overlap += kOverlapGap;

	//動かす物体とそうじゃない物体とで処理を分ける
	if (priorityA > priorityB)
	{
		rbB->AddVec(aToB.Normalize() * overlap);
	}
	else if (priorityA < priorityB)
	{
		rbA->AddVec(aToB.Normalize() * -overlap);
	}
	else
	{
		rbA->AddVec(aToB.Normalize() * -overlap / 2.0f);
		rbB->AddVec(aToB.Normalize() * overlap / 2.0f);
	}
}

void FixNextPosition::FixNextPosCP(const std::shared_ptr<Collidable> collA, const std::shared_ptr<Collidable> collB)
{
	//コライダーデータ
	auto collDataA = std::dynamic_pointer_cast<CapsuleCollider>(collA->m_collisionData);
	auto collDataB = std::dynamic_pointer_cast<PolygonCollider>(collB->m_collisionData);
	//リジッドボディ
	auto rbA = collA->m_rb;
	auto rbB = collB->m_rb;

	//CCDで判定をしているなら
	if (collDataB->IsCCD())
	{
		//当たったポリゴンの情報
		auto lineHit = collDataB->GetLineHit();

		//壁なのか床なのかを見る
		//法線のY成分が大きければ床、小さければ壁
		if (abs(lineHit.Normal.y) < kWallThreshold)
		{
			//壁
			
			//高さ
			float nextY = rbA->GetNextPos().y;
			
			//次の位置
			Vector3 nextPos = lineHit.HitPosition;
			nextPos += Vector3(lineHit.Normal) * (collDataA->GetRadius() + kOverlapGap);

			//始点から終点へのベクトル
			Vector3 sToE = collDataA->GetEndPos() - rbA->GetPos();

			//座標確定
			rbA->SetPos(nextPos);
			rbA->SetPosY(nextY);
			collDataA->SetEndPos(rbA->GetPos() + sToE);
			//移動量リセット
			rbA->SetMoveVec(Vector3::Zero());
		}
		else
		{
			//床

			if (lineHit.Normal.y > 0.0f)
			{
				//次の位置
				Vector3 nextPos = lineHit.HitPosition;
				nextPos.x = 0.0f;
				nextPos.z = 0.0f;
				nextPos.y += collDataA->GetRadius() + kOverlapGap;

				//始点から終点へのベクトル
				Vector3 sToE = collDataA->GetEndPos() - rbA->GetPos();

				//座標確定
				rbA->SetPosY(nextPos.y);
				collDataA->SetEndPos(rbA->GetPos() + sToE);
				//移動量リセット
				rbA->SetVecY(0.0f);
			}
		}

		//CCDリセット
		collDataB->SetIsCCD(false);
	}
	else
	{
		//当たったポリゴンの情報
		auto& hitDim = collDataB->GetHitDim();
		//お互い動かないオブジェクトなら衝突しない(ポリゴンはスタティックなので片方がスタティックなら)
		if (collA->m_priority == Priority::Static)
		{
			// 検出したプレイヤーの周囲のポリゴン情報を開放する
			DxLib::MV1CollResultPolyDimTerminate(hitDim);
			return;
		}

		//カプセルの頭座標と足座標
		Position3 headPos = collDataA->GetNextEndPos(rbA->GetVecWithTS());//移動後
		Position3 legPos = rbA->GetNextPos();//移動後
		//頭より足のほうが低い位置にあるなら入れ替える
		if (headPos.y < legPos.y)
		{
			Position3 temp = legPos;
			legPos = headPos;
			headPos = temp;
		}
		float radius = collDataA->GetRadius();

		//床ポリゴンと壁ポリゴンに分ける
		AnalyzeWallAndFloor(hitDim, legPos);

		//床か天井に当たったか
		bool isFloorAndRoof = !m_floorAndRoof.empty();
		//壁に当たったか
		bool isWall = !m_wall.empty();

		//床と当たったなら
		if (isFloorAndRoof)
		{
			//ジャンプしているなら
			if (collA->m_collState == CollisionState::Jump ||
				rbA->GetVec().y > 0.0f)
			{
				//天井に当たった処理
				HitRoofCP(collA, headPos, radius);
			}
			else
			{
				//床の高さに合わせる
				HitFloorCP(collA, legPos, headPos, radius);
			}
		}
		//壁と当たっているなら
		if (isWall)
		{

			//壁に当たっているので
			collA->SetIsWall(true);

			//補正するベクトルを返す
			Vector3 overlapVec = HitWallCP(headPos, legPos, radius);

			//ベクトルを補正
			rbA->AddVec(overlapVec);
		}


		// 検出したプレイヤーの周囲のポリゴン情報を開放する
		DxLib::MV1CollResultPolyDimTerminate(hitDim);
	}
}

void FixNextPosition::AnalyzeWallAndFloor(MV1_COLL_RESULT_POLY_DIM hitDim, const Vector3& nextPos)
{
	//検出されたポリゴンの数だけ繰り返す
	for (int i = 0; i < hitDim.HitNum;++i)
	{
		// 法線のY成分が大きければ床、小さければ壁
		if (abs(hitDim.Dim[i].Normal.y) < kWallThreshold)
		{
			//壁ポリゴンと判断された場合でも、プレイヤーのY座標＋1.0fより高いポリゴンのみ当たり判定を行う
			//段さで突っかかるのを防ぐため
			if (hitDim.Dim[i].Position[0].y > nextPos.y + 1.0f ||
				hitDim.Dim[i].Position[1].y > nextPos.y + 1.0f ||
				hitDim.Dim[i].Position[2].y > nextPos.y + 1.0f)
			{
				//ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に保存する
				if (m_wall.size() < kMaxHitPolygon)
				{
					//ポリゴンの構造体のアドレスを壁ポリゴン配列に保存
					m_wall.emplace_back(hitDim.Dim[i]);
				}
			}
		}
		//床ポリゴン
		else
		{
			//ポリゴンの数が列挙できる限界数に達していなかったらポリゴン配列に保存
			if (m_floorAndRoof.size() < kMaxHitPolygon)
			{
				//ポリゴンの構造体のアドレスを床ポリゴン配列に保存
				m_floorAndRoof.emplace_back(hitDim.Dim[i]);
			}
		}
	}
}

Vector3 FixNextPosition::OverlapVecSphereAndPoly(const Vector3& nextPos, std::vector<MV1_COLL_RESULT_POLY>& dim, float shortDis)
{
	//垂線を下して近い点を探して最短距離を求める
	float hitShortDis = FLT_MAX;//最短距離
	//法線
	Vector3 nom = {};
	for (auto& poly : dim)
	{
		//内積と法線ベクトルから当たってる座標を求める
		VECTOR bToA = VSub(nextPos.ToDxLibVector(), poly.Position[0]);
		float dot = VDot(poly.Normal, bToA);

		//ポリゴンと当たったオブジェクトが法線方向にいるなら向きを反転
		if ((bToA.y > 0 && poly.Normal.y > 0) || (bToA.y < 0 && poly.Normal.y < 0))
		{
			dot *= -1;
		}
		//当たった座標
		VECTOR hitPos = VAdd(VScale(poly.Normal, dot), nextPos.ToDxLibVector());
		//距離
		float dis = VSize(VSub(hitPos, nextPos.ToDxLibVector()));
		//初回または前回より距離が短いなら
		if (hitShortDis > dis)
		{
			//現状の最短
			hitShortDis = dis;
			//法線
			nom = poly.Normal;
		}
	}
	//押し戻し
	//どれくらい押し戻すか
	float overlap = shortDis - hitShortDis;
	overlap = MathSub::ClampFloat(overlap, 0, shortDis);
	overlap += kOverlapGap;

	return nom.Normalize() * overlap;
}

Vector3 FixNextPosition::HitWallCP(const Vector3& headPos, const Vector3& legPos,float shortDis)
{
	//垂線を下して近い点を探して最短距離を求める
	float hitShortDis = shortDis;//最短距離

	Vector3 top = headPos;
	top.y += shortDis;
	Vector3 bot = legPos;
	bot.y -= shortDis;

	//法線
	Vector3 nom = {};
	for (auto& wall : m_wall)
	{
		//壁かチェック
		if (abs(wall.Normal.y) >= kWallThreshold)continue;
		VECTOR pos1 = wall.Position[0];
		VECTOR pos2 = wall.Position[1];
		VECTOR pos3 = wall.Position[2];
		//最短距離の2乗を返す
		float dis = Segment_Triangle_MinLength_Square(top.ToDxLibVector(), bot.ToDxLibVector(), pos1, pos2, pos3);
		//平方根を返す
		dis = sqrtf(dis);
		//初回または前回より距離が短いなら
		if (hitShortDis > dis)
		{
			//現状の最短
			hitShortDis = dis;
			//法線
			nom = wall.Normal;
		}
	}
	//押し戻し
	//どれくらい押し戻すか
	float overlap = shortDis - hitShortDis;
	overlap = MathSub::ClampFloat(overlap, 0, shortDis);
	overlap += kOverlapGap;
	//正規化
	if (nom.Magnitude() != 0.0f)
	{
		nom = nom.Normalize();
	}
	return nom * overlap;
}


bool FixNextPosition::HitFloorCP(const std::shared_ptr<Collidable> coll, const Vector3& legPos, const Vector3& headPos, float shortDis)
{
	auto rb = coll->m_rb;

	float finalFloorY = -FLT_MAX;// 足の真下にある床の中で最も高いY
	bool hitFloor = false;

	//足の真下レイ（下方向）
	VECTOR rayStart = legPos.ToDxLibVector();
	VECTOR rayEnd = VAdd(rayStart, VGet(0.0f, kCheckUnder, 0.0f));

	for (auto& floor : m_floorAndRoof)
	{
		//下向き法線なら床ではない
		if (floor.Normal.y < 0.0f) continue;

		VECTOR pos1 = floor.Position[0];
		VECTOR pos2 = floor.Position[1];
		VECTOR pos3 = floor.Position[2];

		//足の真下にあるポリゴンと交差チェック
		HITRESULT_LINE res = HitCheck_Line_Triangle(rayStart, rayEnd, pos1, pos2, pos3);

		if (res.HitFlag)
		{
			float hitPosY = res.Position.y;

			// より「高い床」を優先する（坂道で安定）
			if (hitPosY > finalFloorY)
			{
				finalFloorY = hitPosY;
				hitFloor = true;
			}
		}
	}

	//足の真下で床が見つかった
	if (hitFloor)
	{
		float newY = finalFloorY + shortDis + kOverlapGap;

		rb->SetPosY(newY);
		rb->SetVecY(0.0f);
		coll->SetIsFloor(true);

		return true;
	}

	//足の真下に床が無かった場合キャラが坂の腹に刺さっている可能性あり
	//headからleg のラインが床を貫通していないかチェック
	float betweenY = -FLT_MAX;
	bool hitBetween = false;

	VECTOR head = headPos.ToDxLibVector();

	for (auto& floor : m_floorAndRoof)
	{
		if (floor.Normal.y < 0.0f) continue;

		VECTOR pos1 = floor.Position[0];
		VECTOR pos2 = floor.Position[1];
		VECTOR pos3 = floor.Position[2];

		HITRESULT_LINE res = HitCheck_Line_Triangle(head, rayStart, pos1, pos2, pos3);

		if (res.HitFlag)
		{
			if (res.Position.y > betweenY)
			{
				//間の床のY座標を保存
				betweenY = res.Position.y;
				hitBetween = true;
			}
		}
	}

	//headとlegの間で床が見つかった
	if (hitBetween)
	{
		float newY = betweenY + shortDis + kOverlapGap;

		rb->SetPosY(newY);
		rb->SetVecY(0.0f);
		coll->SetIsFloor(true);
		return true;
	}

	return false;
}

void FixNextPosition::HitRoofCP(const std::shared_ptr<Collidable> coll, const Vector3& headPos, float shortDis)
{
	//リジッドボディ
	auto rb = coll->m_rb;
	//垂線を下して近い点を探して最短距離を求める
	float hitShortDis = shortDis;//最短距離
	//当たった中で足元に一番近いY座標に合わせる
	float lowHitPosY = rb->GetPos().y;
	//天井と当たったか
	bool isHitRoof = false;
	for (auto& roof : m_floorAndRoof)
	{
		//上向きの法線ベクトルなら飛ばす
		if (roof.Normal.y > 0.0f)continue;
		// 頭の上にポリゴンがあるかをチェック
		HITRESULT_LINE lineResult = HitCheck_Line_Triangle(headPos.ToDxLibVector(), VAdd(headPos.ToDxLibVector(), VGet(0.0f, kCheckTop, 0.0f)),
			roof.Position[0], roof.Position[1], roof.Position[2]);

		if (lineResult.HitFlag)
		{
			//距離
			float dis = VSize(VSub(lineResult.Position, headPos.ToDxLibVector()));
			//初回または前回より距離が短いなら
			if (hitShortDis > dis)
			{
				isHitRoof = true;
				//現状の最短
				hitShortDis = dis;
			}
		}
	}
	//当たったいるなら
	if (isHitRoof)
	{
		//押し戻し
		//どれくらい押し戻すか
		float overlap = std::abs(shortDis - hitShortDis);
		overlap = MathSub::ClampFloat(overlap, 0, shortDis);
		overlap += kOverlapGap;
		//法線
		Vector3 nom = { 0.0f,-1.0f,0.0f };
		//力を与える
		rb->AddVec(nom * overlap);
	}
}


