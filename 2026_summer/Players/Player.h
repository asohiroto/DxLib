#pragma once
#include "Bases/Character.h"
#include <DxLib.h>
#include <memory>

class Input;
class PlayerMove;
class PlayerDodge;
class Camera;
class MagicShot;
class MagicMissile;
class MagicFury;
class MagicManager;

class Player :
	public Character
{
public:
	enum class PlayerState
	{
		Move,
		Dodge,
		Shot
	};

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
	void SetHit();
	// 当たり判定の色を変更
	void SetColor(int col) { _playerUnit.color = col; }
	// プレイヤーが向いている角度のゲッター
	float GetPlayerAngle() const { return _angle; }
	// 状態を変更
	void StateChange(PlayerState state) { _nowState = state; }

private:
	// プレイヤー
	CharacterData _playerUnit;
	// 移動用クラスの共有ポインタ
	std::shared_ptr<PlayerMove> p_Move;
	// 回避用クラスの共有ポインタ
	std::shared_ptr<PlayerDodge> p_Dodge;
	// モデルを向ける角度　DX_PI_Fで補正
	float _angle;
	// マジックショットの共有ポインタ
	std::shared_ptr<MagicShot> p_Shot;
	// マジックミサイルの共有ポインタ
	std::shared_ptr<MagicMissile> p_Missile;
	// マジックフューリーの共有ポインタ
	std::shared_ptr<MagicFury> p_Fury;
	// プレイヤーの正面ベクトル
	VECTOR _frontVec;
	// 現在のステート
	PlayerState _nowState;
	// ボタンを押している時間
	int _pressFrame;
	// ダメージを受けてからの時間
	int _damagedCount;
};