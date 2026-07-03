#pragma once
#include "CameraBase.h"
#include <memory>
#include <list>
class Actor;
class ActorManager;
class Input;
class PlayerCameraUI;
class PlayerCamera :
	public CameraBase
{
public:
	PlayerCamera();
	~PlayerCamera();
	void Init() override;
	void Update() override;
	void Draw() const override {};
	
	//ロックオン開始
	void StartLockOn(std::weak_ptr<Actor> lockOnTarget);
	//ロックオン終了
	void EndLockOn();
	//ロックオン中か
	bool IsLockOn() const { return m_isLockOn; }
	//プレイヤーの座標
	void SetPlayerPos(const Vector3& pos) { m_playerPos = pos; };
	//プレイヤーのベクトル
	void SetPlayerVec(const Vector3& vec) { m_playerVec = vec; };
	//プレイヤーの向き
	void SetPlayerDir(const Vector3& dir) { m_playerDir = dir; };

	//ポリゴンとの当たり判定をする
	void EnableColl() { m_isThrough = false; };
	void DisableColl() { m_isThrough = true; };

private:
	//ロックオン対象
	std::weak_ptr<Actor> m_lockOnTarget;
	//ロックオン中か
	bool m_isLockOn;
	//プレイヤーの座標
	Vector3 m_playerPos;
	//プレイヤーのベクトル
	Vector3 m_playerVec;
	//プレイヤーの向き
	Vector3 m_playerDir;

	//ロックオン時のカメラが右と左どちらに移動するか
	float m_lockOnSide;
	float m_nextlockOnSide;

	//カメラ操作が最後に行われて何フレーム経過したかをカウント
	float m_lastInputCountFrame;

	//ロックオンUI
	std::weak_ptr<PlayerCameraUI> m_lockOnUI;

	//ポリゴンと当たり判定をするか
	bool m_isThrough;

private:
	//通常時の更新
	void NormalUpdate(Input& input, Vector3& targetPos);
	//ロックオン時の更新
	void LockOnUpdate(Input& input, Vector3& targetPos);

	//向きの更新
	void UpdateDirection(Vector3 newFront);
	//スティックの角度更新
	void UpdateStickAngle(Input& input);
};

