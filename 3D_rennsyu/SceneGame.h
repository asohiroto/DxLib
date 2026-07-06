#pragma once
#include<memory>

class Player;

class SceneGame
{
public:
	SceneGame();
	virtual ~SceneGame();
	void Init();
	void Update();
	void Draw() const;

private:

	std::shared_ptr<Player> p_Player;
	// グリッドの描画
	void DrawGrid() const;
};

