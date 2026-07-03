#pragma once
#include <list>
#include <memory>
#include <map>
#include "../../General/Collision/Collidable.h"

class Actor;
class ActorData;
class Player;
class PlayerCamera;
class CharaStatusData;
class CharacterBase;
class AttackManager;
class EnemyManager;
class Shadow;
class AttackBase;
class ActorManager : public std::enable_shared_from_this<ActorManager>
{
public:
	ActorManager();
	~ActorManager();
	//登録
	void Entry(std::shared_ptr<Actor> actor);
	//解除
	void Exit(std::shared_ptr<Actor> actor);
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw() const;
	//影の描画
	void DrawShadow()const;
	//終了処理
	void End();
	//再スタート(コンテニューが押されたとき)
	void Restart();
	//CSVからアクターを作成
	void CreateActorCSV(const wchar_t* folderName,const wchar_t* fileName);

	//カメラのセット
	void SetPlayerCamera(std::weak_ptr<PlayerCamera> pCamera) { m_pCamera = pCamera; };
	//カメラの参照
	std::weak_ptr<PlayerCamera> GetPlayerCamera()const { return m_pCamera; };

	//攻撃マネージャーのセット
	void SetAttackManager(std::weak_ptr<AttackManager> pAttackManager) { m_pAttackManager = pAttackManager; };
	//攻撃マネージャー参照
	std::weak_ptr<AttackManager> GetAttackManager()const { return m_pAttackManager; };

	//プレイヤーの参照
	std::weak_ptr<Player> GetPlayer()const { return m_pPlayer; };

	//敵マネージャー
	std::shared_ptr<EnemyManager> GetEnemyManager()const { return m_pEnemyManager; };

	//ゲームオーバー
	bool IsGameover()const { return m_isGameover; };

	//ボス戦中
	bool IsBossBattle()const { return m_isBossBattle; };
	void SetIsBossBattle(bool isBattle){ m_isBossBattle = isBattle; };

	//全てのキャラクターを待機状態に
	void AllWait();
	//全てのキャラクターを行動可能に
	void AllOperate();
private:
	//アクター
	std::list<std::shared_ptr<Actor>> m_actors;
	//カメラの参照
	std::weak_ptr<PlayerCamera> m_pCamera;
	//攻撃マネージャーの参照
	std::weak_ptr<AttackManager> m_pAttackManager;
	//プレイヤー
	std::weak_ptr<Player> m_pPlayer;
	//敵マネージャー
	std::shared_ptr<EnemyManager> m_pEnemyManager;
	//影
	std::shared_ptr<Shadow> m_pShadow;
	//ステージを落下したとみなす高さ
	float m_stageFallHeight;
	//ゲームオーバー
	bool m_isGameover;
	//ボス戦中
	bool m_isBossBattle;
private:
	//キャラクターの作成
	std::shared_ptr<CharacterBase> CreateChara(GameTag tag, std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> data);

	//プレイヤーの準備
	void SetUpPlayer(std::shared_ptr<Player> player);

	//削除予定のアクターの削除
	void CheckDelete();

	//ステージを落ちたか
	void CheckStageFall(std::shared_ptr<Actor> actor);
};

