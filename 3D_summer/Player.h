#pragma once
#include"Character.h"
#include<DxLib.h>
#include<memory>

class Input;

class Player :
	public Character
{
public:
	Player();
	~Player() override;
	void Init(int id, int playerH, int subH);
	void Update(float cameraAngle, std::shared_ptr<Input> pInput, std::shared_ptr<Player> pOther);
	void Draw() override;
	// カメラの角度を取得する
	float GetAngle() const { return _angle; }
	// 弱攻撃の当たり判定の中心座標のゲッター
	VECTOR GetWeakCollPos() const { return _weakCollPos; }
	// 攻撃処理
	void AttackProcess(std::shared_ptr<Input> pInput, int type);
	// 攻撃中か
	bool IsAttacking() const { return (_isWeakAttacking || _isStrongAttacking); }
	// 弱攻撃中か
	bool IsWeakAttacking() const { return _isWeakAttacking; }
	// 強攻撃中か
	bool IsStrongAttacking() const { return _isStrongAttacking; }
	// 強攻撃の当たり判定の中心座標のゲッター
	VECTOR GetStrongCollPos() const { return _strongCollPos; }
	// アニメーション変更関数
	void AnimChange(int animIndex);

	// ステートの変更
	void ChangeState(PlayerState next);
	// 待機状態の更新
	void UpdateIdle(std::shared_ptr<Input> pInput);
	// 移動状態の更新
	void UpdateMove(std::shared_ptr<Input> pInput, std::shared_ptr<Player> pOther, float cameraAngle);
	// 弱攻撃状態の更新
	void UpdateWAttack(std::shared_ptr<Input> pInput);
	// 強攻撃状態の更新
	void UpdateSAttack(std::shared_ptr<Input> pInput);
	// 回避状態の更新
	void UpdateDodge(std::shared_ptr<Input> pInput);
	// 当たり判定の処理
	void CollProcess(std::shared_ptr<Player> pOther);
	// ヒットストップのフレーム数を取得し、その値をリセットする
	int HitstopRequest();
	// アタッチされているアニメーション番号のゲッター
	int GetAttachAnimIndex() const { return _attachAnimIndex; }
	// モデルからアニメーションをデタッチする処理
	void DetachAnim();

private:
	// Y軸方向の移動
	float _movementY;
	// 動く方向
	VECTOR _move;
	// カメラの回転を方向に変換した値
	VECTOR _movementDirection;
	// カメラの回転行列
	MATRIX _rotMatrix;
	// モデルの回転行列
	MATRIX _modelRotMatrix;
	// モデルが向く角度
	float _angle;
	// 回避中か
	bool _isDodge;
	// 現在の回避量
	float _dodgeMovement;
	// 回避秒数（回避距離算出に使用）
	int _dodgeCount;
	// 回避の方向
	VECTOR _dodgeDir;
	//攻撃可能間隔のカウンタ
	int _attackCount;
	// 弱攻撃中か
	bool _isWeakAttacking;
	// 弱攻撃判定の中心座標
	VECTOR _weakCollPos;
	// 弱攻撃の回転方向の一時保存用
	VECTOR _attackDirection;
	// プレイヤー1、２の判別用
	int _idTemp;
	// ダメージを受けたか
	bool _isDamaged;
	// 弱攻撃ダメージ判定用のカウンタ
	int _wDamagedCount;
	// 強攻撃ダメージ判定用のカウンタ
	int _sDamagedCount;
	// 【デバッグ】プレイヤーの当たり判定の色
	int _playerColor;
	// 強攻撃判定の中心座標
	VECTOR _strongCollPos;
	// 強攻撃中か
	bool _isStrongAttacking;
	// アタッチしているアニメーションの番号
	int _attachAnimIndex;
	// アニメーション再生用のカウンタ
	float _animCount;
	// アニメーションの総再生時間
	float _totalTime;
	// 変更元のアニメーションの番号
	int _prevAttachAnimIndex;
	// ブレンド率
	float _blendRate;
	// アニメーションを変化させるか
	bool _isAnimChange;
	// 変更先のアニメーションの番号
	int _nextAttachAnimIndex;
	// プレイヤーステート
	PlayerState _state;
	// ダッシュ中か
	bool _isDash;
	// プレイヤーの移動速度
	float _playerSpeed;
	// アニメーションの再生速度
	float _animSpeed;
	// 被弾時にヒットストップするフレーム数
	int _hitstopRequestFrame;
	// 攻撃モーションの振りかぶりを待機するカウント
	int _attackWindUpCount;
	// 回避可能までのクールタイム
	int _playerDodgeCoolCount;
	// 弱攻撃ダメージクールダウン中か
	bool _isWDamageCooldown;
	// 強攻撃ダメージクールダウン
	bool _isSDamageCooldown;
};

