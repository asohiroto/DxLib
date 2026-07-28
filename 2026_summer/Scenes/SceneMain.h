#pragma once
#include "SceneBase.h"
#include<memory>

class Player;
class Camera;
class Input;

class SceneMain :
	public SceneBase
{
public:
	SceneMain();
	virtual ~SceneMain() override;
	void Init() override;
	void End() override;
	void Update() override;
	void Draw() override;
	// グリッドを描画する関数
	void DrawGrid();

private:
	// プレイヤークラスの共有ポインタ
	std::shared_ptr<Player> p_Player;
	// カメラクラスの共有ポインタ
	std::shared_ptr<Camera> p_Camera;
	// インプットクラスの共有ポインタ
	std::shared_ptr<Input> p_Input;
};