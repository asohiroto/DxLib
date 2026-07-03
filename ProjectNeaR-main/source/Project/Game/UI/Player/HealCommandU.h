#pragma once
#include "../UIBase.h"
#include "../../../General/Math/MyMath.h"
#include <memory>
class Player;
class HealCommandU :
    public UIBase
{
public:
	HealCommandU(std::weak_ptr<Player> pPlayer);
	~HealCommandU();
	//更新
	void Update() override;
	//描画
	void Draw()const override;
private:
	//プレイヤー
	std::weak_ptr<Player> m_pPlayer;
	//ハンドル
	int m_backHandle;
	int m_fontHandle;
	//一フレーム前の回復可能回数
	int m_beforeFullRecoveryNum;
	//回復した際にUIを少し下に動かす際の角度
	float m_moveAngle;
	//プレイヤーがピンチの時点滅させるためのカウント
	int m_pinchBlinkCount;
	//描画位置
	Vector2 m_imagePos;
	Vector2 m_textPos;
};

