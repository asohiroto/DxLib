#include "Boss2Arm.h"
#include "../EnemyStateBase.h"
#include "Boss2StateIdle.h"
#include "../../../ActorManager.h"
#include "../../../../../General/game.h"
#include "../../../../../General/Collision/CapsuleCollider.h"
#include "../../../../../General/Collision/Rigidbody.h"
#include "../../../../../General/Collision/Collidable.h"
#include "../../../../../General/CharaStatus.h"
#include "../../../../../General/Input.h"
#include "../../../../../General/Model.h"
#include "../../../../../General/Animator.h"
#include "../../../../Attack/AttackManager.h"
#include "../../../../Camera/PlayerCamera.h"
#include "../../../../../General/CSV/CSVDataLoader.h"
#include "../../../../../General/CSV/EnemyAttackKeyData.h"
#include "../../../../UI/Enemy/NormalEnemyHPUI.h"
#include <DxLib.h>
#include <cmath>
#include <cassert>

namespace
{
	//敵キャラのパスデータ数
	constexpr int kPathNum = 3;
}

Boss2Arm::Boss2Arm(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager) :
	EnemyBase(actorData, charaStatusData, pActorManager),
	m_modelHandle(-1),
	m_startFrameIndex(0),
	m_endFrameIndex(0)
{
	//ボス
	m_isBoss = false;
}
Boss2Arm::~Boss2Arm()
{
}
void Boss2Arm::Init()
{
	//Physicsに登録
	Collidable::Init();

	//モデルは必要ないので
	if (m_model)
	{
		//モデル削除
		m_model->End();
		m_model = nullptr;
	}

	//待機状態にする(最初はプレイヤー内で状態を初期化するがそのあとは各状態で遷移する
	auto thisPointer = std::dynamic_pointer_cast<Boss2Arm>(shared_from_this());

	//体力UI
	auto enemyHPUI = std::make_shared<NormalEnemyHPUI>(m_charaStatus, thisPointer);
	enemyHPUI->Init();
	//体力回復
	m_charaStatus->FullRecovery();

	//腕の位置更新
	UpdateArmPos();
	//移動量リセット
	m_rb->SetVec(Vector3::Zero());

}

void Boss2Arm::Update()
{
	//非活動中は更新しない
	if (!m_isActive)return;
	//死亡したか
	if (!m_isDelete)
	{
		m_isDelete = m_charaStatus->IsDead();
	}
	if (m_isDelete)return;
	//腕の位置更新
	UpdateArmPos();
	//移動量リセット
	m_rb->SetVec(Vector3::Zero());
	//ロックオンの位置
	UpdateLockOnViewPos();
	
}

void Boss2Arm::Complete()
{
	//腕の位置更新
	UpdateArmPos();
	//移動量リセット
	m_rb->SetVec(Vector3::Zero());
	if (m_charaStatus->IsHit())
	{
#if _DEBUG
		printf("敵の残りの体力 : %d\n", m_charaStatus->GetNowHP());
#endif
	}
}


void Boss2Arm::End()
{
	//登録解除
	Collidable::End();
	//削除
	m_isDelete = true;
}

void Boss2Arm::SetArmIndex(int startIndex, int endIndex)
{
	m_startFrameIndex = startIndex;
	m_endFrameIndex = endIndex;
}

void Boss2Arm::SetArmModel(int modelHandle)
{
	m_modelHandle = modelHandle;
}


void Boss2Arm::UpdateArmPos()
{
	//位置
	m_rb->SetPos(MV1GetFramePosition(m_modelHandle,m_startFrameIndex));
	std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->SetEndPos(MV1GetFramePosition(m_modelHandle, m_endFrameIndex));
}
