#pragma once
#include "../../CharacterBase.h"
#include <memory>
#include <vector>
#include <string>
#include "../Player.h"
class ActorManager;
class Collidable;
class Rigidbody;
class ActorData;
class CharaStatusData;
class AttackData;
class BulletAttack;
class CSVDataLoader;
class AttackBase;

class Pod :
    public CharacterBase
{
public:
    Pod(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData,
		std::weak_ptr<ActorManager> pActorManager,std::weak_ptr<Player> pPlayer);
    ~Pod();
	//初期化処理
	void Init()override;
	//更新処理
	void Update() override;
	//衝突イベント関数
	void OnCollide(const std::shared_ptr<Collidable> other)override;
	//描画
	void Draw()const override;
	void DrawShadow()const override;
	//更新処理による結果の確定
	void Complete() override;
	//終了処理
	void End()override;

	//ステートにアクセスさせる関数
	//リジッドボディ
	std::shared_ptr<Rigidbody> GetRb() const { return m_rb; }
	//Player座標
	Vector3 GetPlayerPos()const;
	//Player向き
	Vector3 GetPlayerDir()const;
	//ターゲット
	TargetInfo GetPlayerTargetInfo()const;

	//カメラの向き
	Vector3 GetCameraDir()const;
	//弾
	std::vector<std::shared_ptr<BulletAttack>> GetBullets()const { return m_bullets; };

	//滑空状態
	bool IsGliding()const { return m_isGliding; };
	void SetIsGliding(bool isGliding) { m_isGliding = isGliding; };

	//アニメーション取得
	std::string GetAnim(std::wstring state) const;

	//半径
	float GetRadius()const { return 0.0f; };
private:
	//プレイヤーの参照
	std::weak_ptr<Player> m_pPlayer;

	//弾を打つ際にその都度newすると負荷がかかり重くなるのであらかじめ作っておく
	std::vector<std::shared_ptr<BulletAttack>> m_bullets;

	//滑空状態
	bool m_isGliding;
};

