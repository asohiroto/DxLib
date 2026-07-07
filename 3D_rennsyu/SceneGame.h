#pragma once
#include<vector>
#include<memory>

class Player;
class Camera;
class GameObject;

class SceneGame
{
public:
	SceneGame();
	virtual ~SceneGame();
	void Init();
	void Update();
	void Draw() const;

	void LoadGameObject();

private:

	std::shared_ptr<Player> p_Player;
	std::vector<std::shared_ptr<GameObject>> p_GameObjects;


	// グリッドの描画
	void DrawGrid() const;
};

