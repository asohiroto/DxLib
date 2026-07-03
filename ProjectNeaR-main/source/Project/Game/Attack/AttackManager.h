#pragma once
#include <list>
#include <memory>
#include <map>
#include "../../General/Collision/Collidable.h"

class Actor;
class AttackBase;
class PlayerCamera;
class AttackManager : public std::enable_shared_from_this<AttackManager>
{
public:
	AttackManager();
	~AttackManager();
	//登録
	void Entry(std::shared_ptr<AttackBase> attack);
	//解除
	void Exit(std::shared_ptr<AttackBase> attack);
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw()const;
	//終了処理
	void End();

	//ヒットストップ
	void HitStop(std::shared_ptr<AttackBase> attack);

	//カメラ
	void SetPlayerCamera(std::weak_ptr<PlayerCamera> pPlayerCamera);
private:
	//攻撃のリスト
	std::list<std::shared_ptr<AttackBase>> m_attacks;

	//ヒットストップフレーム
	int m_hitStopFrame;

	//ヒットストップ中
	bool m_isHitStop;

	//カメラの参照
	std::weak_ptr<PlayerCamera> m_pPlayerCamera;
private:
	//削除チェック
	void CheckDelete();
};

