#pragma once
#include"SceneBase.h"
#include<memory>

class Player;

class SceneGame :
	public SceneBase
{
public:
	SceneGame();
	~SceneGame() override;
	void Init() override;
	void Update() override;
	void Draw() override;

private:
	// プレイヤークラスの共有ポインタ
	std::shared_ptr<Player> p_Player;
};

