#pragma once
#include "../CharacterBase.h"
#include "../../../../General/Math/MyMath.h"
#include "../../../../General/CSV/AnimData.h"
#include "../../../../General/CSV/AttackData.h"
#include "../../../../General/TargetInfo.h"
#include <memory>
#include <map>
#include <string>
#include <list>

class ActorManager;
class CharacterStateBase;
class CharaStatusData;
class Input;
class ActorData;
class PlayerCamera;
class Weapon;
class CSVDataLoader;
class AttackBase;
class EnemyBase;
class AvoidColl;
class Player :
	public CharacterBase
{
public:
	Player(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, std::weak_ptr<ActorManager> pActorManager);
	virtual ~Player();
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

	//回復処理
	void Heal(Input& input);
	
	//カメラの回転量
	Quaternion GetCameraRot()const;

	//ジャンプ回数
	int GetJumpNum() const { return m_jumpNum; };
	void AddJumpNum();
	//ジャンプ可能
	bool IsJumpable()const;

	//回避可能
	bool IsAvoidable()const { return m_isAvoidable; };
	void SetIsAvoidable(bool isAvoidable){ m_isAvoidable = isAvoidable; };

	//空中攻撃可能か
	bool IsAirAttacked()const { return m_isAirAttacked; };
	void SetIsAirAttacked(bool isAttack){ m_isAirAttacked = isAttack; };

	//落下しているか
	bool IsFall()const;

	//無敵時間
	void SetNoDamageFrame(float m_frame);
	//ジャスト回避後の無敵フレーム
	void UpdateJustAvoid();

	//持ってる武器情報
	AnimData::WeaponType GetHaveWeaponType()const { return m_haveWeaponType; };

	//武器の参照
	void SetSword(std::weak_ptr<Weapon> weapon,bool isLightSword);
	std::weak_ptr<Weapon> GetWeapon(AnimData::WeaponType type)const;

	//片手剣を持つ
	void HaveLightSword();
	//大剣を持つ
	void HaveBigSword();
	//武器を収める
	void PutAwaySword();

	//アニメーションの検索
	std::string GetAnim(std::wstring state) const;

	//滑空状態か
	bool IsGliding()const;

	//描画
	void SetIsDraw(bool isDraw) { m_isDraw = isDraw; };

	//ターゲット情報
	TargetInfo GetTargetInfo()const { return m_targetInfo; };

	//半径
	float GetRadius()const override;

	//ゴールしたか
	bool IsGoal()const { return m_isGoal; };
	void SetIsGoal(bool isGoal) { m_isGoal = isGoal; };

	//ジャスト回避数
	int GetTotalJustAvoidNum()const { return m_totalJustAvoidNum; };
	void AddJustAvoidNum() { ++m_totalJustAvoidNum; };
	void ResetJustAvoidNum() { m_totalJustAvoidNum = 0; };

	//強制待機状態か
	bool IsWait()const;

	//リスポーン地点
	void SetRespawnPos(Vector3 pos);
	void Respawn();

	//回復可能回数
	int GetFullRecoveryNum()const { return m_fullRecoveryNum; };

	//スタート状態にする
	void SetStartState();
	bool IsStartState()const;

private:
	//カメラ
	std::weak_ptr<PlayerCamera> GetPlayerCamera()const;

	//ターゲットを探す
	void SearchTarget(Input& input, std::shared_ptr<PlayerCamera> camera, const std::list<std::shared_ptr<EnemyBase>>& enemys);

	//最も近い敵をロックオン
	void LockOnNearestEnemy(std::shared_ptr<PlayerCamera> camera, const std::list<std::shared_ptr<EnemyBase>>& enemys);

	//スティックを倒した方向にある敵を検索して切り替え
	std::shared_ptr<EnemyBase> FindTargetInDirection(bool rightDir, std::shared_ptr<PlayerCamera> camera, 
		std::shared_ptr<EnemyBase> currentTarget, const std::list<std::shared_ptr<EnemyBase>>& enemys);

	//ターゲットリセット
	void ResetTarget(std::shared_ptr<PlayerCamera> camera);

	//攻撃を喰らった際のポストエフェクト
	void UpdateHit();

	//武器を収める
	void UpdatePutAwayWeapon();
private:
	//回避判定
	std::shared_ptr<AvoidColl> m_avoidColl;

	//ジャンプ回数
	int m_jumpNum;
	//回避可能
	bool m_isAvoidable;
	//空中攻撃を行ったか
	bool m_isAirAttacked;

	//ジャスト回避数
	int m_totalJustAvoidNum;

	//リスポーン地点
	Vector3 m_respawnPos;
	
	//回復可能回数
	int m_fullRecoveryNum;

	//無敵時間(ジャスト回避成功時数フレーム無敵)
	bool m_isJustAvoided;
	float m_noDamageFrame;

	//武器
	std::weak_ptr<Weapon> m_pLightSword;
	std::weak_ptr<Weapon> m_pBigSword;

	//武器の状態
	AnimData::WeaponType m_haveWeaponType;

	//武器を収めるまでのフレームをカウント
	float m_putAwayCountFrame;
	//描画しない
	bool m_isDraw;
	
	//現在のターゲット情報
	TargetInfo m_targetInfo;

	//グリッジ
	float m_glitchFrame;
	float m_glitchCountFrame;
	float m_glitchScale;
	float m_glitchSpeed;
	float m_glitchkStrengt;
	bool m_isHitGlitch;

	//ゴールしたか
	bool m_isGoal;
};

