#pragma once
#include<memory>

class Player;

class UIManager
{
public:
	UIManager();
	~UIManager();
	void Init();
	void Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Player> pPlayerSub);
	void Draw(std::shared_ptr<Player> pPlayer, std::shared_ptr<Player> pPlayerSub);

private:
	/// <summary>
	/// HPゲージを描画する関数
	/// </summary>
	/// <param name="playerType">いずれのプレイヤーか</param>
	/// <param name="pPlayer">プレイヤーの共有ポインタ</param>
	void DrawHpGauge(int playerType, std::shared_ptr<Player> pPlayer);

private:
	// 表示するプレイヤー１のHPの数値
	int _playerDispHp;
	// 表示するプレイヤー２のHPの数値
	int _subDispHp;
};