#include "SelectPlayer.h"
#include "SelectPlayerStateIdle.h"
#include "../Enemy/EnemyManager.h"
#include "../Enemy/EnemyBase.h"
#include "../../../../General/game.h"
#include "../../../../General/Collision/CapsuleCollider.h"
#include "../../../../General/Collision/Rigidbody.h"
#include "../../../../General/Collision/Collidable.h"
#include "../../../../General/CharaStatus.h"
#include "../../../../General/Input.h"
#include "../../../../General/Model.h"
#include "../../../../General/Animator.h"
#include "../../../../General/Collision/Physics.h"
#include "../../ActorManager.h"
#include "../../../Attack/AttackManager.h"
#include "../../../Camera/PlayerCamera.h"
#include "../../../../General/CSV/CSVDataLoader.h"
#include "../../../../General/CSV/CSVData.h"
#include <DxLib.h>
#include <cmath>
#include <cassert>
#include "../../../../General/ShaderPostProcess.h"
#include "../../../../Main/Application.h"
#include "../../../../General/AssetManager.h"
#include "../../../../Game/UI/Player/PlayerHPUI.h"
#include "../../../../Game/UI/Player/HealCommandU.h"
#include "../../../../Game/UI/UIManager.h"

namespace
{
	//プレイヤーのパスデータ数
	constexpr int kPathNum = 5;
}

SelectPlayer::SelectPlayer(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager) :
	CharacterBase(actorData, charaStatusData, Shape::Capsule, pActorManager)
{

}

SelectPlayer::~SelectPlayer()
{
}

void SelectPlayer::Init()
{
	//必要なパスを取得
	auto& csvLoader = CSVDataLoader::GetInstance();
	auto pathData = csvLoader.LoadCSV(m_actorData->GetCSVPathData().c_str()).front()->GetData();

	assert(pathData.size() >= kPathNum);
	//共通初期化
	CharacterBase::Init(pathData[0].c_str(), pathData[1].c_str(), pathData[2].c_str(),pathData[3].c_str(), pathData[4].c_str());

	//待機状態にする
	auto thisPointer = std::dynamic_pointer_cast<SelectPlayer>(shared_from_this());
	m_state = std::make_shared<SelectPlayerStateIdle>(thisPointer);
	//状態を変化する
	m_state->ChangeState(m_state);

}

void SelectPlayer::Update()
{
	//共通処理
	CharacterBase::Update();
	//モデルの座標更新
	m_model->SetPos(m_rb->m_pos.ToDxLibVector());
	m_model->ApplyMat();
}


void SelectPlayer::OnCollide(const std::shared_ptr<Collidable> other)
{
}

void SelectPlayer::Draw() const
{
	m_model->Draw();
}

void SelectPlayer::DrawShadow() const
{
	Draw();
}

void SelectPlayer::Complete()
{
}


void SelectPlayer::End()
{
	//モデル削除
	m_model->End();
	//登録解除
	Collidable::End();
}

float SelectPlayer::GetRadius() const
{
	return std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetRadius();
}
