#pragma once
#include "Bases/SceneBase.h"
#include<memory>

class Player;
class Camera;
class Input;
class Enemy;
class Collision;
class MagicManager;
class MagicCollision;
class UIManager;

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
	// エネミークラスの共有ポインタ
	std::shared_ptr<Enemy> p_Enemy;
	// コリジョンクラスの共有ポインタ
	std::shared_ptr<Collision> p_Coll;
	// 魔法管理クラスの共有ポインタ
	std::shared_ptr<MagicManager> p_MManager;
	// 魔法のコリジョンクラスの共有ポインタ
	std::shared_ptr<MagicCollision> p_MColl;
	// UI管理クラスのポインタ
	std::shared_ptr<UIManager> p_UI;
};
