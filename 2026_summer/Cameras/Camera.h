#pragma once
#pragma once
#include<memory>
#include <DxLib.h>

class Player;
class Input;
class Enemy;

class Camera
{
public:
	Camera();
	virtual ~Camera();
	void Init();
	void End();
	void Update(const std::shared_ptr<Player>& pPlayer, const std::shared_ptr<Enemy>& pEnemy, const std::shared_ptr<Input>& pInput);
	void Draw();
	// カメラの水平方向の角度のゲッター
	float GetCameraYaw() const { return _cameraYaw; }
	// カメラモードのゲッター
	bool GetCameraMode() const { return _cameraMode; }
	// カメラモードのセッター
	void SetCameraMode(bool mode) { _cameraMode = mode; }
	// プレイヤーから敵への水平方向
	VECTOR GetEnemyDirection() const { return _dirToEnemy; }
	// 画面中心からのレイキャストが敵に当たっているかのゲッター
	bool IsRayHitEnemy() const { return _isRayHitEnemy; }
	// レイキャストが命中した敵の座標のゲッター
	VECTOR GetRayHitEnemyPos() const { return _rayHitEnemyPos; }

private:
	// 通常のカメラ
	void NormalCam(const std::shared_ptr<Player>& pPlayer);
	// ロックオン時のカメラ
	void LockOnCam(const std::shared_ptr<Player>& pPlayer, const std::shared_ptr<Enemy>& pEnemy);
	// 画面中心から正面方向へレイキャストを行い、敵に当たっているか判定する
	void CheckRayCastHitEnemy(const std::shared_ptr<Enemy>& pEnemy);

private:
	// 水平方向のカメラの回転角度
	float _cameraYaw;
	// 表示用のカメラの水平方向回転角度
	float _dispCameraYaw;
	// カメラの座標
	VECTOR _cameraPos;
	// 注視点
	VECTOR _targetPos;
	// カメラモード
	// trueでロックオンモード
	// falseで通常カメラモード
	bool _cameraMode;
	// 敵までの水平方向
	VECTOR _dirToEnemy;

	// Update呼び出し回数のカウンタ(初回フレーム判定用)
	int _cameraCount;

	// 画面中心からのレイキャストが敵に当たっているか
	bool _isRayHitEnemy;
	// レイキャストが命中した敵の座標
	VECTOR _rayHitEnemyPos;
};