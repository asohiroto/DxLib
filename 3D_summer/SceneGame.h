#pragma once
#include<memory>

class Player;
class Camera;
class Input;
class SceneManager;

class SceneGame
{
public:
	SceneGame();
	~SceneGame();
	void Init(SceneManager* pManager);
	void Update();
	void Draw();
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
	// シーンマネージャーの生ポインタ（相互参照による、スマートポインタのデリート不可のため）
	SceneManager* p_Manager;
	// ヒットストップ更新処理用のカウンタ
	int _hitstopCount;
	// カメラがズームしたときの距離
	int _zoomDistance;
	// スカイドームのハンドル
	int _skyDomeH;
	// カメラの境界座標
	float _border;
};

