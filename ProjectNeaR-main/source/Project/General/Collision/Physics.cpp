#include "Physics.h"
#include "CollisionChecker.h"
#include "FixNextPosition.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "PolygonCollider.h"
#include "Rigidbody.h"
#include "../../Game/Attack/AttackBase.h"
#include "../../Game/Actor/Character/Enemy/EnemyBase.h"
#include "../../Main/Application.h"
#include <cassert>

namespace
{
	//確認回数
	constexpr int kTryNum = 3;

	//重力
	const Vector3 kGroundGravity = { 0.0f, -10.0f, 0.0f };
	const Vector3 kAirGravity = { 0.0f, -0.5f, 0.0f };
}

void Physics::Init()
{
	m_isUpdate = true;
	m_collChecker = std::make_shared<CollisionChecker>();
	m_collProcessor = std::make_shared<FixNextPosition>();
}

void Physics::Entry(std::shared_ptr<Collidable> collidable)
{
	//すでに登録されているならしない
	auto it = std::find(m_collidables.begin(), m_collidables.end(), collidable);
	if (it != m_collidables.end())return;
	//コライダーの追加
	m_collidables.emplace_back(collidable);
}

void Physics::Exit(std::shared_ptr<Collidable> collidable)
{
	//登録されていないならしない
	auto it = std::find(m_collidables.begin(), m_collidables.end(), collidable);
	if (it == m_collidables.end())return;
	m_collidables.erase(it);
}

void Physics::Update()
{
	//更新をしないなら
	if (!m_isUpdate)return;
	//タイムスケールの初期化
	InitTimeScale();
	//重力
	Gravity();
	//床と壁のとの当たったフラグを初期化
	for (auto& collidable : m_collidables)
	{
		collidable->ResetHitFlag();
	}
	//遅延処理用
	std::list<OnCollideInfo> onCollideInfo;
	
	//攻撃当たり判定チェック(ノックバックが発生する可能性があるので先に処理)
	CheckAttackColl(onCollideInfo);

	// 当たり通知
	for (auto& collInfo : onCollideInfo)
	{
		collInfo.OnCollide();
	}
	//リセット
	onCollideInfo.clear();

	//当たり判定処理
	CheckCollidable(onCollideInfo);

	// 当たり通知
	for (auto& collInfo : onCollideInfo)
	{
		collInfo.OnCollide();
	}

	//位置確定
	for (auto& coll : m_collidables)
	{
		//位置を確定
		coll->Complete();
	}
}

void Physics::CheckCollidable(std::list<Physics::OnCollideInfo>& onCollideInfo)
{
	bool isOneMore = false;
	for (int i = 0; i < kTryNum; ++i)
	{
		//当たり判定をチェック
		for (auto& collA : m_collidables)
		{
			//当たり判定を行わないなら飛ばす
			if (collA->GetGameTag() == GameTag::None)continue;
			if (collA->m_isThrough)continue;
			//攻撃ならスキップ
			if (collA->GetGameTag() == GameTag::Attack)continue;
			//回避判定ならスキップ
			if (collA->GetGameTag() == GameTag::Avoid)continue;

			//敵の場合
			if (collA->GetGameTag() == GameTag::Enemy)
			{
				auto enemy = std::dynamic_pointer_cast<EnemyBase>(collA);
				//非活動中は当たり判定を行わない
				if (!enemy->IsActive())continue;
			}

			for (auto& collB : m_collidables)
			{
				//自分とは当たり判定をしない
				if (collA == collB)continue;
				//当たり判定を行わないなら飛ばす
				if (collB->GetGameTag() == GameTag::None)continue;
				if (collB->m_isThrough)continue;
				//攻撃ならスキップ
				if (collB->GetGameTag() == GameTag::Attack)continue;
				//エリア同士ならスキップ
				if (collA->GetGameTag() == GameTag::Area && 
					collB->GetGameTag() == GameTag::Area)
				{
					continue;
				}
				//回避判定ならスキップ
				if (collB->GetGameTag() == GameTag::Avoid)continue;

				//敵の場合
				if (collB->GetGameTag() == GameTag::Enemy)
				{
					auto enemy = std::dynamic_pointer_cast<EnemyBase>(collB);
					//非活動中は当たり判定を行わない
					if (!enemy->IsActive())continue;
				}

				//どちらもトリガーではないか
				bool isBothNoTrigger = !collA->m_isTrigger && !collB->m_isTrigger;
				if (isBothNoTrigger)
				{
					//動かないもの同士なら
					if (collA->m_priority == Priority::Static &&
						collB->m_priority == Priority::Static)
					{
						continue;
					}
				}

				//当たってるなら
				if (m_collChecker->IsCollide(collA, collB))
				{
					//どちらもトリガーなではないなら
					if (isBothNoTrigger)
					{
						//もう一度ループする(押し戻しによって別のオブジェクトに当たっているかもしれないので)
						isOneMore = true;

						//衝突処理
						m_collProcessor->FixNextPos(collA, collB);
					}
					//これまでにこの組み合わせで当たった情報があるかをチェック
					bool isCollInfo = false;
					for (const auto& item : onCollideInfo)
					{
						// 既に通知リストに含まれていたら呼ばない
						if (item.owner == collA && item.colider == collB ||
							item.owner == collB && item.colider == collA)
						{
							isCollInfo = true;
						}
					}
					//ない場合
					if (!isCollInfo)
					{
						onCollideInfo.emplace_back(OnCollideInfo{ collA, collB });
						onCollideInfo.emplace_back(OnCollideInfo{ collB, collA });
					}
				}
			}
		}

		//繰り返す必要がない場合
		if (!isOneMore)
		{
			break;
		}
	}
}

void Physics::CheckAttackColl(std::list<Physics::OnCollideInfo>& onCollideInfo)
{
	//当たり判定をチェック
	for (auto& collA : m_collidables)
	{
		//当たり判定を行わないなら飛ばす
		if (collA->GetGameTag() == GameTag::None)continue;
		if (collA->m_isThrough)continue;
		//攻撃以外ならスキップ
		if (collA->GetGameTag() != GameTag::Attack)continue;

		for (auto& collB : m_collidables)
		{
			//自分とは当たり判定をしない
			if (collA == collB)continue;
			//当たり判定を行わないなら飛ばす
			if (collB->GetGameTag() == GameTag::None)continue;
			if (collB->m_isThrough)continue;
			//エリアは無視
			if (collB->GetGameTag() == GameTag::Area)continue;

			//攻撃同士
			if (collA->GetGameTag() == GameTag::Attack &&
				collB->GetGameTag() == GameTag::Attack)
			{
				//同じタイプの攻撃なら当たらない
				if (std::dynamic_pointer_cast<AttackBase>(collA)->GetOwnerTag() ==
					std::dynamic_pointer_cast<AttackBase>(collB)->GetOwnerTag())
				{
					continue;
				}
			}

			//敵の場合
			if (collB->GetGameTag() == GameTag::Enemy)
			{
				auto enemy = std::dynamic_pointer_cast<EnemyBase>(collB);
				//非活動中は当たり判定を行わない
				if (!enemy->IsActive())continue;
			}

			//どちらもトリガーではないか
			bool isBothNoTrigger = !collA->m_isTrigger && !collB->m_isTrigger;
			if (isBothNoTrigger)
			{
				//動かないもの同士なら
				if (collA->m_priority == Priority::Static &&
					collB->m_priority == Priority::Static)
				{
					continue;
				}
			}

			//当たってるなら
			if (m_collChecker->IsCollide(collA, collB))
			{
				//どちらもトリガーなではないなら
				if (isBothNoTrigger)
				{
					//衝突処理
					m_collProcessor->FixNextPos(collA, collB);
				}
				//これまでにこの組み合わせで当たった情報があるかをチェック
				bool isCollInfo = false;
				for (const auto& item : onCollideInfo)
				{
					// 既に通知リストに含まれていたら呼ばない
					if (item.owner == collA && item.colider == collB ||
						item.owner == collB && item.colider == collA)
					{
						isCollInfo = true;
					}
				}
				//ない場合
				if (!isCollInfo)
				{
					onCollideInfo.emplace_back(OnCollideInfo{ collA, collB });
					onCollideInfo.emplace_back(OnCollideInfo{ collB, collA });
				}
			}
		}
	}

}

void Physics::Reset()
{
	m_collidables.clear();
}

std::list<std::weak_ptr<Collidable>> Physics::GetAreaXCollidable(float startX, float endX)
{
	std::list<std::weak_ptr<Collidable>> collList;
	for (auto& collidable : m_collidables)
	{
		if (collidable->GetGameTag() == GameTag::None)continue;
		auto collPos = collidable->m_rb->m_pos;
		//範囲内にいたら
		if (collPos.x > startX && collPos.x < endX)
		{
			collList.emplace_back(collidable);
		}
	}
	return collList;
}

std::list<std::weak_ptr<Collidable>> Physics::RayCast(const Vector3& startPos, const Vector3& endPos)
{
	std::list<std::weak_ptr<Collidable>> collList;
	for (auto& collidable : m_collidables)
	{
		if (collidable->GetGameTag() == GameTag::None)continue;
		if (collidable->GetGameTag() == GameTag::Attack)continue;
		if (collidable->GetGameTag() == GameTag::Area)continue;
		if (collidable->m_isTrigger)continue;
		if (collidable->m_isThrough)continue;
		auto shape = collidable->GetShape();
		if (shape == Shape::None)continue;
		//球が当たっているなら
		if (shape == Shape::Sphere)
		{
			if (HitCheck_Line_Sphere(startPos.ToDxLibVector(), endPos.ToDxLibVector(),
				collidable->m_rb->m_pos.ToDxLibVector(), std::dynamic_pointer_cast<SphereCollider>(collidable->m_collisionData)->GetRadius()))
			{
				collList.emplace_back(collidable);
			}
		}
		//カプセル
		else if(shape == Shape::Capsule)
		{
			auto cap = std::dynamic_pointer_cast<CapsuleCollider>(collidable->m_collisionData);
			if (HitCheck_Capsule_Capsule(startPos.ToDxLibVector(), endPos.ToDxLibVector(), 0.0f,
				collidable->m_rb->m_pos.ToDxLibVector(), cap->GetEndPos().ToDxLibVector(), cap->GetRadius()))
			{
				collList.emplace_back(collidable);
			}
		}
		//ポリゴン
		else if(shape == Shape::Polygon)
		{
			auto hitData = MV1CollCheck_Line(std::dynamic_pointer_cast<PolygonCollider>(collidable->m_collisionData)->GetModelHandle(), -1,
				startPos.ToDxLibVector(), endPos.ToDxLibVector());
			if (hitData.HitFlag)
			{
				collList.emplace_back(collidable);
			}
		}
	}
	return collList;
}

Vector3 Physics::GetCameraRatCastNearEndPos(const Vector3& targetPos, const Vector3& cameraPos)
{
	auto hitColls = RayCast(targetPos, cameraPos);
	float shortDis = 1000000.0f;
	Vector3 hitPos = cameraPos;
	for (auto& hitColl : hitColls)
	{
		if (hitColl.expired())continue;
		auto pHitColl = hitColl.lock();
		//ポリゴンかチェック
		if (pHitColl->GetShape() != Shape::Polygon)continue;
		if (pHitColl->m_isThrough)continue;
		if (pHitColl->m_isTrigger)continue;
		MV1_COLL_RESULT_POLY result = MV1CollCheck_Line(std::dynamic_pointer_cast<PolygonCollider>(pHitColl->m_collisionData)->GetModelHandle(), -1,
			targetPos.ToDxLibVector(), cameraPos.ToDxLibVector());
		if (!result.HitFlag)continue;
		//最短かどうか
		float dis = (Vector3(result.HitPosition) - targetPos).Magnitude();
		if (shortDis > dis)
		{
			shortDis = dis;
			hitPos = result.HitPosition;
		}
	}
	return hitPos;
}

void Physics::InitTimeScale()
{
	float timeScale = Application::GetInstance().GetTimeScale();
	for (auto& collidable : m_collidables)
	{
		//自分のタイムスケールを優先しないなら
		if (!collidable->m_rb->IsMyTimeScale())
		{
			//グローバルのタイムスケールをセット
			collidable->SetTimeScale(timeScale);
		}
	}
}

void Physics::Gravity()
{
	auto& app = Application::GetInstance();
	
	for (auto& collidable : m_collidables)
	{
		//重力を受けるか
		if (!collidable->m_rb->m_isGravity)continue;
		auto rb = collidable->m_rb;
		//地上にいるときと空中にいるときで重力の大きさを変える
		auto gravity = kGroundGravity;
		//地上にいない場合
		if (!collidable->IsFloor() || 
			collidable->m_collState == CollisionState::Jump ||
			collidable->m_collState == CollisionState::Fall)
		{
			gravity = kAirGravity;
		}
		//時間補正付きで重力を加える
		rb->m_vec += gravity * rb->GetMyTimeScale();
	}
}