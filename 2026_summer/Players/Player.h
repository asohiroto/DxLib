#pragma once
#include "Bases//Character.h"
#include <DxLib.h>
#include <memory>

class Input;
class PlayerMove;
class Camera;
class MagicShot;
class MagicManager;

class Player :
	public Character
{
public:
	Player();
	virtual ~Player() override;
	void Init() override;
	void End()override;
	void Update(std::shared_ptr<Input> pInput, std::shared_ptr<Camera> pCamera, std::shared_ptr<MagicManager> pManager);
	void Draw() override;
	// プレイヤー座標のゲッター
	VECTOR GetPos() const { return _playerUnit.pos; }
	// 座標のセッター
	void SetPos(VECTOR pos) { _playerUnit.pos = pos; }
	// プレイヤーデータのゲッター
	CharacterData GetPlayerData() const { return _playerUnit; }
	// 接触中処理
	void SetHit(bool judge) { _playerUnit.isHit = judge; }
	// 当たり判定の色を変更
	void SetColor(int col) { _playerUnit.color = col; }
	// モデルを回転させる関数
	void ModelRotate(int rx, int ry);

private:
	// プレイヤー
	CharacterData _playerUnit;
	// 移動用クラスの共有ポインタ
	std::shared_ptr<PlayerMove> p_Move;
	// カメラの向いている角度
	float _cameraAngle;
	// モデルを向ける角度
	float _angle;
	// マジックショットの共有ポインタ
	std::shared_ptr<MagicShot> p_Shot;
	// プレイヤーの正面ベクトル
	VECTOR _frontVec;
};