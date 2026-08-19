#pragma once
#include "Bases/SceneBase.h"
#include<memory>

class Player;
class Camera;
class Input;
class Enemy;
class EnemyManager;
class Collision;
class MagicManager;
class MagicCollision;
class UIManager;
class SkyDome;

class SceneMain :
	public SceneBase
{
public:
	SceneMain();
	virtual ~SceneMain() override;
	void Init(int playerH, int enemyH, int domeH);
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
	// エネミー管理クラスの共有ポインタ
	std::shared_ptr<EnemyManager> p_EManager;
	// コリジョンクラスの共有ポインタ
	std::shared_ptr<Collision> p_Coll;
	// 魔法管理クラスの共有ポインタ
	std::shared_ptr<MagicManager> p_MManager;
	// 魔法のコリジョンクラスの共有ポインタ
	std::shared_ptr<MagicCollision> p_MColl;
	// UI管理クラスのポインタ
	std::shared_ptr<UIManager> p_UI;
	// スカイドームのポインタ
	std::unique_ptr<SkyDome> p_Dome;

	int a, b, c;
};
