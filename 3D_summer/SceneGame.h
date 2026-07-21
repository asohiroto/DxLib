#pragma once
#include"SceneBase.h"
#include<memory>

class Player;
class Camera;
class Input;

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
	// サブプレイヤー用のプレイヤークラスの共有ポインタ
	std::shared_ptr<Player> p_PlayerSub;
	// カメラクラスの共有ポインタ
	std::shared_ptr<Camera> p_Camera;
	// サブプレイヤー用のカメラクラスの共有ポインタ
	std::shared_ptr<Camera> p_CameraSub;
	// インプットクラスの共有ポインタ
	std::shared_ptr<Input> p_Input;
	// サブプレイヤーのインプットクラスの共有ポインタ　
	std::shared_ptr<Input> p_InputSub;
	// ヒットストップ更新処理用のカウンタ
	int _hitstopCount;
};

