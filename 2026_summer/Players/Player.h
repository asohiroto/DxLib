#pragma once
#include "Bases/Character.h"
#include "Effects/EffectHandles.h"
#include "Sounds/SoundInfo.h"
#include "PlayerMove.h"
#include "PlayerDodge.h"
#include "Magics/MagicShot.h"
#include "Magics/MagicMissile.h"
#include "Magics/MagicFury.h"
#include "Magics/MagicBeam.h"
#include "Anims/AnimManager.h"
#include <DxLib.h>
#include <memory>

class Input;
class Camera;
class MagicManager;

class Player : public Character
{
public:
	Player();
	virtual ~Player() override;
	void Init(int handle, EffectHandles playerMagics, SeHandles se);
	void End()override;
	void Update(Input& pInput, const std::shared_ptr<Camera>& pCamera, const std::shared_ptr<MagicManager>& pManager);
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
	// 必殺技をチャージする
	void SetUltCharge(int amount);
	// プレイヤーのモデルを回転
	void SetRotate(float angle) { _angle = angle; MV1SetRotationXYZ(_playerUnit.modelH, VGet(0.0f, angle, 0.0f)); };

private:
	// 状況に応じてステートを変化させる
	void UpdateState(Input& pInput);
	// プレイヤーの今のステートを返す
	Character::CharacterState GetNowState() const { return _playerUnit.nowState; }

private:
	// プレイヤー
	CharacterData _playerUnit;
	// 移動用クラス
	PlayerMove _Move;
	// 回避用クラス
	PlayerDodge _Dodge;
	// モデルを向ける角度　DX_PI_Fで補正
	float _angle;
	// マジックショット
	MagicShot _Shot;
	// マジックミサイル
	MagicMissile _Missile;
	// マジックフューリー
	MagicFury _Fury;
	// マジックビーム
	MagicBeam _Beam;
	// アニメーションマネージャー
	AnimManager _AManager;

	// プレイヤーの正面ベクトル
	VECTOR _frontVec;
	// ボタンを押している時間
	int _pressFrame;
	// 必殺技ボタン(A)を押している時間
	int _ultPressFrame;
	// ダメージを受けてからの時間
	int _damagedCount;
	// マジックサークルのハンドル
	int _magicCircleH;
	// エフェクト再生中のハンドル
	int _circlePlayingH;
	// 必殺技チャージ中の魔法陣再生中のハンドル
	int _ultCirclePlayingH;
	// 目標地点のハンドル
	int _targetPlayingH;
	// ジャスト回避エフェクトの再生中ハンドル
	int _dodgeEffectPlayingH;
	// 回避中か
	bool _isDodge;
	// SE
	SeHandles _gameSE;

	// プレイヤーの魔法用エフェクトハンドル一式
	EffectHandles _playerMagics;
};