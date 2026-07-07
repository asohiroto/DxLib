#pragma once
#include"SceneBase.h"
#include<memory>

class Player;
class Camera;

class SceneGame :
	public SceneBase
{
public:
	SceneGame();
	~SceneGame() override;
	void Init() override;
	void Update() override;
	void Draw() override;
	// 足元にグリッドを表示する
	void DrawGrid() const;
private:
	// プレイヤークラスの共有ポインタ
	std::shared_ptr<Player> p_Player;
	// カメラクラスの共有ポインタ
	std::shared_ptr<Camera> p_Camera;
};

