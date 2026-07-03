#include "NormalEnemy.h"
#include "EnemyStateIdle.h"
#include "../../../../General/game.h"
#include "../../../../General/Collision/CapsuleCollider.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/Collision/Collidable.h"
#include "../../../../General/CharaStatus.h"
#include "../../../../General/Input.h"
#include "../../../../General/Model.h"
#include "../../../../General/Animator.h"
#include "../../../Attack/AttackManager.h"
#include "../../../Camera/PlayerCamera.h"
#include "../../../../General/CSV/CSVDataLoader.h"
#include "../../../UI/Enemy/NormalEnemyHPUI.h"
#include <DxLib.h>
#include <cmath>
#include <cassert>

namespace
{
	//敵キャラのパスデータ数
	constexpr int kPathNum = 6;
}

NormalEnemy::NormalEnemy(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager):
	EnemyBase(actorData, charaStatusData, pActorManager)
{
	//ボスではない
	m_isBoss = false;
}
NormalEnemy::~NormalEnemy()
{
}
void NormalEnemy::Init()
{
	//必要なパスを取得
	auto& csvLoader = CSVDataLoader::GetInstance();
	auto pathData = csvLoader.LoadCSV(m_actorData->GetCSVPathData().c_str()).front()->GetData();

	assert(pathData.size() >= kPathNum);
	//共通初期化
	CharacterBase::Init(pathData[0].c_str(), pathData[1].c_str(), pathData[2].c_str(), pathData[3].c_str(), pathData[4].c_str());

	//攻撃のキーを取得
	InitAttackKey(csvLoader, pathData[5]);

	//目の位置
	InitEyeIndex(csvLoader, pathData[6]);

	//待機状態にする(最初はプレイヤー内で状態を初期化するがそのあとは各状態で遷移する
	auto thisPointer = std::dynamic_pointer_cast<NormalEnemy>(shared_from_this());
	m_state = std::make_shared<EnemyStateIdle>(thisPointer);
	//状態を変化する
	m_state->ChangeState(m_state);
	//モデルの高さ調整
	m_model->SetModelHeightAdjust(-m_actorData->GetCollRadius());

	//体力UI
	auto enemyHPUI = std::make_shared<NormalEnemyHPUI>(m_charaStatus, thisPointer);
	enemyHPUI->Init();
	//体力回復
	m_charaStatus->FullRecovery();

	//位置
	m_rb->SetPos(m_actorData->GetPos());
	Vector3 endPos = m_rb->m_pos;
	endPos.y += m_actorData->GetCollHeight();
	std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->SetEndPos(endPos);//カプセルの移動
	//移動量リセット
	m_rb->SetVec(Vector3::Zero());

	//モデルの座標更新
	m_model->SetPos(m_rb->m_pos.ToDxLibVector());
	m_model->ApplyMat();
}


void NormalEnemy::Complete()
{
	if (!m_isActive)
	{
		m_rb->SetVec(Vector3::Zero());
		//モデルの座標更新
		m_model->SetPos(m_rb->m_pos.ToDxLibVector());
		m_model->ApplyMat();
		return;
	}

	m_rb->SetPos(m_rb->GetNextPos());
	Vector3 endPos = m_rb->m_pos;
	endPos.y += m_actorData->GetCollHeight();
	std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->SetEndPos(endPos);//カプセルの移動
	//モデルの座標更新
	m_model->SetPos(m_rb->m_pos.ToDxLibVector());
	m_model->ApplyMat();

	if (m_charaStatus->IsHit())
	{
#if _DEBUG
		printf("敵の残りの体力 : %d\n", m_charaStatus->GetNowHP());
#endif
	}
}


void NormalEnemy::End()
{
	//モデル削除
	m_model->End();
	//登録解除
	Collidable::End();
}