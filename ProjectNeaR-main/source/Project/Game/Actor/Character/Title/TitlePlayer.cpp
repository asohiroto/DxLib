#include "TitlePlayer.h"
#include "TitlePlayerStateIdle.h"
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

TitlePlayer::TitlePlayer(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager) :
	CharacterBase(actorData, charaStatusData, Shape::Capsule, pActorManager)
{

}

TitlePlayer::~TitlePlayer()
{
}

void TitlePlayer::Init()
{
	//必要なパスを取得
	auto& csvLoader = CSVDataLoader::GetInstance();
	auto pathData = csvLoader.LoadCSV(m_actorData->GetCSVPathData().c_str()).front()->GetData();

	assert(pathData.size() >= kPathNum);
	//共通初期化
	CharacterBase::Init(pathData[0].c_str(), pathData[1].c_str(), pathData[2].c_str(), pathData[3].c_str(), pathData[4].c_str());

	//待機状態にする
	auto thisPointer = std::dynamic_pointer_cast<TitlePlayer>(shared_from_this());
	m_state = std::make_shared<TitlePlayerStateIdle>(thisPointer);
	//状態を変化する
	m_state->ChangeState(m_state);

}

void TitlePlayer::Update()
{
	//共通処理
	CharacterBase::Update();
	//モデルの座標更新
	m_model->SetPos(m_rb->m_pos.ToDxLibVector());
	m_model->ApplyMat();
}


void TitlePlayer::OnCollide(const std::shared_ptr<Collidable> other)
{
}

void TitlePlayer::Draw() const
{
	m_model->Draw();
}

void TitlePlayer::DrawShadow() const
{
	Draw();
}

void TitlePlayer::Complete()
{
}


void TitlePlayer::End()
{
	//モデル削除
	m_model->End();
	//登録解除
	Collidable::End();
}

float TitlePlayer::GetRadius() const
{
	return std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetRadius();
}
