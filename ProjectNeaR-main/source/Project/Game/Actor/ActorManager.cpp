#include "ActorManager.h"
#include "Actor.h"
#include <DxLib.h>
#include <cassert>
#include "../../General/CSV/CSVDataLoader.h"
#include "../../General/CSV/ActorData.h"
#include "../../General/CSV/CharaStatusData.h"
#include "../../General/CSV/PodData.h"
#include "../../General/Fader.h"
#include "../../General/Shadow.h"
#include "Stage/StageObject.h"
#include "Stage/Goal.h"
#include "Stage/Sky.h"
#include "Character/CharacterBase.h"
#include "Character/Player/Player.h"
#include "Character/Select/SelectPlayer.h"
#include "Character/Title/TitlePlayer.h"
#include "Character/Enemy/NormalEnemy.h"
#include "Character/Player/Weapon/Weapon.h"
#include "Character/Player/Pod/Pod.h"
#include "Character/Enemy/EnemyManager.h"
#include "Character/Enemy/Boss1/Boss1.h"
#include "Character/Enemy/Boss2/Boss2.h"
#include "Character/Enemy/Boss3/Boss3.h"
#include "Character/Enemy/Boss4/Boss4.h"
#include "Stage/CheckPoint.h"


namespace
{
	//CSVデータ名
	const std::wstring kCharaStatusDataName = L"CharaStatusData";
	const std::wstring kWeaponDataName = L"Player/WeaponData";
	const std::wstring kPodDataName = L"Player/PodData";
	//特殊処理アクター名
	const std::wstring kBoss1Name = L"Boss1";
	const std::wstring kBoss2Name = L"Boss2";
	const std::wstring kBoss3Name = L"Boss3";
	const std::wstring kBoss4Name = L"Boss4";
	const std::wstring kGoalName = L"Goal";
	const std::wstring kHiganbanaName = L"Higanbana";
	const std::wstring kLightSwordName = L"LightSword";
	const std::wstring kPlayerName = L"Player";
	const std::wstring kSelectPlayerName = L"SelectPlayer";
	const std::wstring kTitlePlayerName = L"TitlePlayer";

}

ActorManager::ActorManager():
	m_stageFallHeight(0.0f),
	m_isGameover(false),
	m_isBossBattle(false)
{
	
}

ActorManager::~ActorManager()
{
}

//アクターを追加
void ActorManager::Entry(std::shared_ptr<Actor> actor)
{
	//すでに登録されているならしない
	auto it = std::find(m_actors.begin(), m_actors.end(), actor);
	if (it != m_actors.end())return;
	//アクターの初期化
	actor->Init();
	//アクターを追加
	m_actors.emplace_back(actor);

	//敵なら
	if (actor->GetGameTag() == GameTag::Enemy)
	{
		m_pEnemyManager->Entry(std::dynamic_pointer_cast<EnemyBase>(actor));
	}
}

void ActorManager::Exit(std::shared_ptr<Actor> actor)
{
	//登録されていないならしない
	auto it = std::find(m_actors.begin(), m_actors.end(), actor);
	if (it == m_actors.end())return;
	actor->End();
	m_actors.erase(it);

	//敵なら
	if (actor->GetGameTag() == GameTag::Enemy)
	{
		m_pEnemyManager->Exit(std::dynamic_pointer_cast<EnemyBase>(actor));
	}
}


void ActorManager::Init()
{
	//敵マネージャーの作成
	m_pEnemyManager = std::make_shared<EnemyManager>(shared_from_this());
	//影
	m_pShadow = std::make_shared<Shadow>();
	//フラグリセット
	m_isGameover = false;
	m_isBossBattle = false;
}

void ActorManager::Update()
{
	//敵マネージャーの更新
	m_pEnemyManager->Update();
	//アクターの更新
	for (auto& actor : m_actors)
	{
		//高さをチェック
		CheckStageFall(actor);
		//更新
		actor->Update();
	}
	//削除
	CheckDelete();
}

void ActorManager::Draw() const
{
	//影描画
	m_pShadow->DrawShadow(shared_from_this());
	//アクターの描画
	for (auto& actor : m_actors)
	{
		actor->Draw();
	}

#if _DEBUG
	m_pEnemyManager->DebugDraw();
#endif
}
void ActorManager::DrawShadow() const
{
	//アクターの影描画
	for (auto& actor : m_actors)
	{
		actor->DrawShadow();
	}
}

void ActorManager::End()
{
	//アクターの終了処理
	std::list<std::shared_ptr<Actor>> deleteActer;
	for (auto& actor : m_actors)
	{
		deleteActer.emplace_back(actor);
	}
	for (auto& actor : deleteActer)
	{
		Exit(actor);
	}
	deleteActer.clear();
}
void ActorManager::Restart()
{
	//アクターの初期化
	for (auto& actor : m_actors)
	{
		actor->Init();
	}

	//エネミーマネージャーの初期化
	m_pEnemyManager->Init();

	//フラグリセット
	m_isGameover = false;
	m_isBossBattle = false;
}

void ActorManager::CreateActorCSV(const wchar_t* folderName, const wchar_t* fileName)
{
	//CSVを読み込む
	auto& csvLoader = CSVDataLoader::GetInstance();
	std::wstring path = folderName + std::wstring(L"/") + fileName;
	auto csvDatas = csvLoader.LoadCSV(path.c_str());
	if (csvDatas.empty())return;
	//キャラクターのステータスを読み込む
	std::wstring charaStatusPath = folderName + std::wstring(L"/") + kCharaStatusDataName;
	auto csvStatusDatas = csvLoader.LoadCSV(charaStatusPath.c_str());

	//アクターの作成
	for (auto& data : csvDatas)
	{
		if (!data)continue;
		//データを変換
		auto actorData = std::make_shared<ActorData>(data);
		//アクターを作成
		std::shared_ptr<Actor> actor;

		auto aa = actorData->GetActorType();


		if (actorData->GetActorType() == ActorData::ActorType::Character)
		{
			for (auto& statusData : csvStatusDatas)
			{
				auto tStatusData = std::make_shared<CharaStatusData>(statusData);
				//IDが一致するなら
				if (tStatusData->GetID() == actorData->GetActorID())
				{
					//キャラクター作成
					actor = CreateChara(actorData->GetGameTag(), actorData, tStatusData);
				}
			}
		}
		else if (actorData->GetActorType() == ActorData::ActorType::Stage)
		{
			if (actorData->GetName() == kGoalName)
			{
				//ゴール
				actor = std::make_shared<Goal>(actorData, shared_from_this());
			}
			else if (actorData->GetGameTag() == GameTag::Sky)
			{
				//空
				actor = std::make_shared<Sky>(actorData, shared_from_this());
			}
			else
			{
				if (actorData->GetName() == kHiganbanaName)
				{
					actorData->SetScale(actorData->GetScale() * 100.0f);
				}

				//ステージ
				actor = std::make_shared<StageObject>(actorData, shared_from_this());
			}
			//低い高さを保持
			m_stageFallHeight = MathSub::Min(m_stageFallHeight, actor->GetPos().y);
		}
		else if (actorData->GetActorType() == ActorData::ActorType::CheckPoint)
		{
			//チェックポイント
			actor = std::make_shared<CheckPoint>(actorData, shared_from_this());
		}

		//登録
		Entry(actor);

		//プレイヤーなら
		if (actorData->GetGameTag() == GameTag::Player &&
			actorData->GetName() == kPlayerName)
		{
			//武器を持たせる
			SetUpPlayer(std::dynamic_pointer_cast<Player>(actor));
		}
	}
}


void ActorManager::AllWait()
{
	for (auto actor : m_actors)
	{
		//全てのキャラクターを待機状態に
		auto gameTag = actor->GetGameTag();
		if (gameTag == GameTag::Player ||
			gameTag == GameTag::Enemy)
		{
			if (!std::dynamic_pointer_cast<CharacterBase>(actor))continue;
			std::dynamic_pointer_cast<CharacterBase>(actor)->Wait();
		}
	}
}

void ActorManager::AllOperate()
{
	for (auto actor : m_actors)
	{
		//全てのキャラクターを行動可能状態に
		auto gameTag = actor->GetGameTag();
		if (gameTag == GameTag::Player ||
			gameTag == GameTag::Enemy)
		{

			if (!std::dynamic_pointer_cast<CharacterBase>(actor))continue;
			std::dynamic_pointer_cast<CharacterBase>(actor)->Operate();
		}
	}
}

std::shared_ptr<CharacterBase> ActorManager::CreateChara(GameTag tag, std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData)
{
	std::shared_ptr<CharacterBase> chara;
	//プレイヤー
	if (tag == GameTag::Player)
	{
		if (actorData->GetName() == kPlayerName)
		{
			chara = std::make_shared<Player>(actorData, charaStatusData, shared_from_this());
		}
		else if (actorData->GetName() == kSelectPlayerName)
		{
			chara = std::make_shared<SelectPlayer>(actorData, charaStatusData, shared_from_this());
		}
		else if (actorData->GetName() == kTitlePlayerName)
		{
			chara = std::make_shared<TitlePlayer>(actorData, charaStatusData, shared_from_this());
		}
	}
	else if (tag == GameTag::Enemy)
	{
		if (actorData->GetName() == kBoss1Name)
		{
			//ボス1
			chara = std::make_shared<Boss1>(actorData, charaStatusData, shared_from_this());
		}
		else if (actorData->GetName() == kBoss2Name)
		{
			//ボス2
			chara = std::make_shared<Boss2>(actorData, charaStatusData, shared_from_this());
		}
		else if (actorData->GetName() == kBoss3Name)
		{
			//ボス3
			chara = std::make_shared<Boss3>(actorData, charaStatusData, shared_from_this());
		}
		else if (actorData->GetName() == kBoss4Name)
		{
			//ボス4
			chara = std::make_shared<Boss4>(actorData, charaStatusData, shared_from_this());
		}
		else
		{
			//敵キャラクター
			chara = std::make_shared<NormalEnemy>(actorData, charaStatusData, shared_from_this());
		}
	}
	//nullチェック
	assert(chara);
	return chara;
}

void ActorManager::SetUpPlayer(std::shared_ptr<Player> player)
{
	//プレイヤーの参照
	m_pPlayer = player;

	//CSVを読み込む
	auto& csvLoader = CSVDataLoader::GetInstance();
	std::wstring path = kWeaponDataName;
	auto csvDatas = csvLoader.LoadCSV(path.c_str());
	//武器作成
	for (auto& data : csvDatas)
	{
		if (!data)continue;
		//データを変換
		auto weaponData = std::make_shared<ActorData>(data);
		//武器を作成
		std::shared_ptr<Weapon> weapon = std::make_shared<Weapon>(weaponData,shared_from_this());

		//マネージャーにエントリー
		Entry(weapon);

		//武器をセット
		if (weaponData->GetName() == kLightSwordName)
		{
			//片手剣
			player->SetSword(weapon,true);
		}
		else
		{
			//大剣
			player->SetSword(weapon,false);
		}
	}
	//ポッドのデータ読み込み
	path = kPodDataName;
	csvDatas = csvLoader.LoadCSV(path.c_str());
	//データを変換
	auto podData = std::make_shared<PodData>(csvDatas.back());

	//必要な情報のみをセットしていく
	//アクターデータ
	auto podActorData = std::make_shared<ActorData>();
	podActorData->SetScale(podData->GetScale());
	podActorData->SetModelPath(podData->GetModelPath());
	auto podStatusData = std::make_shared<CharaStatusData>();
	podStatusData->SetAT(podData->GetAT());
	//ポッドを作成
	std::shared_ptr<Pod> pod = std::make_shared<Pod>(podActorData, podStatusData, shared_from_this(), player);

	//マネージャーにエントリー
	Entry(pod);
}

//消滅フラグをチェックして削除
void ActorManager::CheckDelete()
{
	//消滅フラグが立っているものを削除
	m_actors.remove_if([=](const std::shared_ptr<Actor>& actor) {
		if (actor->IsDelete()) {
			//プレイヤーは無視
			if (actor->GetGameTag() == GameTag::Player)
			{
				//ゲームオーバー
				m_isGameover = true;
				return false;
			}
			actor->End();
			//敵ならエネミーマネージャーからも削除
			if (actor->GetGameTag() == GameTag::Enemy)
			{
				m_pEnemyManager->Exit(std::dynamic_pointer_cast<EnemyBase>(actor));
			}
			return true;
		}
		return false;
		});
}

void ActorManager::CheckStageFall(std::shared_ptr<Actor> actor)
{
	if (actor->GetPos().y < m_stageFallHeight)
	{
		//ステージ外に落下しているなら
		if (actor->GetGameTag() == GameTag::Player)
		{
			auto& fader = Fader::GetInstance();

			//完全に暗くなったら
			if (fader.IsFinishFadeOut())
			{
				//リスポーン
				std::dynamic_pointer_cast<Player>(actor)->Respawn();
				//だんだん明るく
				fader.FadeIn();
			}
			if (!fader.IsFadeOutNow() && !fader.IsFadeInNow())
			{
				//だんだん暗くなる
				fader.FadeOut();
			}
		}
		else if (actor->GetGameTag() == GameTag::Enemy)
		{
			//削除
			actor->Delete();
		}
	}
}