#pragma once
#include<memory>
#include<DxLib.h>

class Player;
class Input;

class Camera
{
public:
	Camera();
	virtual ~Camera();
	void Init();
	void Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Player> pOther, std::shared_ptr<Input> pInput);
	void Draw(int playerNum);
	// カメラの水平方向の角度を渡す
	float GetCameraYaw() const { return _cameraYaw; }
	// カメラの距離のセッター
	void SetCameraDistance(float dist) { _cameraDistance = dist; }

private:
	/// <summary>
	/// 球面線形補間をさせる関数
	/// </summary>
	/// <param name="p0">出発点（正規化済み）</param>
	/// <param name="p1">到着点（正規化済み）</param>
	/// <param name="t">線形補間度</param>
	/// <returns></returns>
	VECTOR Slerp(VECTOR p0, VECTOR p1, float t);

private:
	// 水平方向の角度
	float _cameraYaw;
	// 垂直方向の角度
	float _cameraPitch;
	// カメラ位置の保存用（描画用）
	VECTOR _cameraPos;
	// 注視点の保存用（描画用）
	VECTOR _targetPos;
	// 線形補完後の向き
	VECTOR _smoothedForward;
	// カメラの線形補間度
	float _cameraLerpRate;
	// カメラモードがいずれか
	bool _changedCameraMode;
	// 1フレーム目の処理用
	bool _initialize;
	// 操作感確認用
	bool _isTest;
	// 現在フレームでのカメラの位置からターゲットまでのベクトル
	VECTOR _nowDir;
	// Slerp後のベクトル
	VECTOR _slerpedDir;
	// 実際のカメラの水平方向回転角
	float _dispCameraYaw;
	// 実際のカメラの垂直方向回転角
	float _dispCameraPitch;
	// カメラからプレイヤーまでの距離
	float _cameraDistance;
};

