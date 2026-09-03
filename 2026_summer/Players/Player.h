#pragma once
#include "Bases/Character.h"
#include "Effects/EffectHandles.h"
#include "Sounds/SoundInfo.h"
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
class MagicBeam;
class AnimManager;

class Player : public Character
{
public:
	Player();
	virtual ~Player() override;
	void Init(int handle, EffectHandles playerMagics, SeHandles se);
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
	void SetHit(int damage);
	// 当たり判定の色を変更
	void SetColor(int col) { _playerUnit.color = col; }
	// プレイヤーが向いている角度のゲッター
	float GetPlayerAngle() const { return _angle; }
	// 最大HPのゲッター
	int GetMaxHp() const { return _playerUnit.maxHp; }
	// 現在HPのゲッター
	int GetNowHp() const { return _playerUnit.hp; }
	// 回避を行っているか
	bool IsDodge()const { return _isDodge; }
	// 最大MPのゲッター
	float GetMaxMp() const { return _playerUnit.maxMp; }
	// 現在MPのゲッター
	float GetNowMp() const { return _playerUnit.mp; }
	// ジャスト回避成功によって引き起こされる効果
	void JustDodgeEffect();
	// 現在の必殺技のチャージ量
	float GetNowCharge() const { return _playerUnit.ultCharge; }
	// 必殺技の最大チャージ量
	float GetMaxCharge() const { return _playerUnit.maxUltCharge; }
	// プレイヤーの今のステートを返す
	Character::CharacterState GetNowState() const { return _playerUnit.nowState; }
	// 必殺技をチャージする
	void SetUltCharge(int amount);
	// プレイヤーのモデルを回転
	void SetRotate(float angle) { _angle = angle; MV1SetRotationXYZ(_playerUnit.modelH, VGet(0.0f, angle, 0.0f)); };

private:
	// 状況に応じてステートを変化させる
	void UpdateState(std::shared_ptr<Input> pInput);

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
	// マジックビームの共有ポインタ
	std::shared_ptr<MagicBeam> p_Beam;
	// アニメーションマネージャーの共有ポインタ
	std::shared_ptr<AnimManager> p_AManager;

	// プレイヤーの正面ベクトル
	VECTOR _frontVec;
	// ボタンを押している時間
	int _pressFrame;
	// ダメージを受けてからの時間
	int _damagedCount;
	// マジックサークルのハンドル
	int _magicCircleH;
	// エフェクト再生中のハンドル
	int _circlePlayingH;
	// 目標地点のハンドル
	int _targetPlayingH;
	// 回避中か
	bool _isDodge;
	// SE
	SeHandles _gameSE;

	EffectHandles _playerMagics;
};