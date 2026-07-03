#pragma once
#include "../HPUIBase.h"
#include "../../../General/Math/MyMath.h"
#include <memory>
class CharaStatus;
class EnemyBase;
class NormalEnemyHPUI :
	public HPUIBase
{
public:
	NormalEnemyHPUI(std::shared_ptr<CharaStatus> charaStatus, std::weak_ptr<EnemyBase> pEnemy);
	~NormalEnemyHPUI();
	//更新
	void Update() override;
	//描画
	void Draw()const override;
private:
	//持ち主
	std::weak_ptr<EnemyBase> m_pEnemy;
	//敵の座標
	Vector3 m_enemyViewPos;
	//体力の変動があった場合フレームをカウント
	int m_changeHPCountFrame;
};

