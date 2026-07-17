#include "Camera.h"
#include"Player.h"
#include"Input.h"
#include"GameDefine.h"
#include<DxLib.h>
#include<algorithm>
#include<cmath>

using namespace GameDefine;

Camera::Camera() :
	_cameraYaw(0.0f),
	_cameraPitch(0.5f),
	_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	_targetPos(VGet(0.0f, 0.0f, 0.0f)),
	_smoothedForward(VGet(0.0f, 0.0f, 0.0f)),
	_cameraLerpRate(0),
	_changedCameraMode(false),
	_initialize(false),
	_nowDir(VGet(0.0f, 0.0f, 0.0f)),
	_slerpedDir(VGet(0.0f, 0.0f, 0.0f)),
	_isTest(false)
{

}

Camera::~Camera()
{

}

void Camera::Init()
{

}

void Camera::Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Input> pInput)
{
	_cameraLerpRate = 0;

	int rx = pInput->GetRightStickX();
	int ry = pInput->GetRightStickY();

	// 注視点
	VECTOR targetPos = VGet(0.0f, 0.0f, 0.0f);

	// Bボタンでカメラモードを切り替える
	if (pInput->IsTrigger(PAD_INPUT_A))
	{
		_changedCameraMode = !_changedCameraMode;
	}

	// プレイヤーが存在していることを確認してから、注視点をプレイヤーの座標に
	if (pPlayer != nullptr)
	{
		targetPos = pPlayer->GetPos();

		// カメラモードがONのときは、注視点に高さと前方向の補正をかける
		if (_changedCameraMode)
		{
			// 注視点を足元から一定の高さに補正
			targetPos.y += CAMERA_TARGET_HEIGHT;

			// プレイヤーの向いている角度から、補間前の前方向ベクトルを求める
			float playerAngle = pPlayer->GetAngle();
			VECTOR rawForward = VGet(sinf(playerAngle), 0, cosf(playerAngle));

			// 移動入力があるときだけ、前方向の追従を有効にする
			if (pInput->IsTiltingL())
			{
				_cameraLerpRate = 0.03f;
			}

			// 前方向ベクトルを線形補間で滑らかに追従させる
			_smoothedForward = VAdd
			(
				VScale(_smoothedForward, 1.0f - _cameraLerpRate),
				VScale(rawForward, _cameraLerpRate)
			);

			// 補間済みの前方向へ、注視点を一定距離押し出す
			targetPos = VAdd(targetPos, VScale(_smoothedForward, CAMERA_TARGET_FORWARD_OFFSET));
		}
		else
		{
			targetPos.y += CAMERA_TARGET_HEIGHT;
		}
	}

	// 入力に応じて水平、垂直方向にカメラの回転角度を決定
	// （-1000～1000の入力値を-1.0～1.0に正規化して使う）
	_cameraYaw = _cameraYaw + CAMERA_YAW_SPEED * std::clamp(rx / 1000.0f, -1.0f, 1.0f);
	_cameraPitch = _cameraPitch + -CAMERA_PITCH_SPEED * std::clamp(ry / 1000.0f, -1.0f, 1.0f);

	// カメラの垂直方向の回転限界
	_cameraPitch = std::clamp(_cameraPitch, -CAMERA_PITCH_LIMIT, -0.2f);

	// 回転角度から求めた、カメラの目標位置
	VECTOR cameraPos = VGet(0.0f, 0.0f, 0.0f);
	cameraPos.x = CAMERA_DISTANCE * std::cosf(_cameraPitch) * std::sinf(_cameraYaw);
	cameraPos.y = CAMERA_DISTANCE * std::sinf(_cameraPitch);
	cameraPos.z = CAMERA_DISTANCE * std::cosf(_cameraPitch) * std::cosf(_cameraYaw);

	cameraPos = VAdd(cameraPos, targetPos);


	if (_isTest/*_changedCameraMode*/)
	{
		// 初回フレームは補間を行わず、そのまま位置を確定させる
		// （_slerpedDirに有効な初期値を入れておかないと、以降の補間が壊れるため）
		if (!_initialize)
		{
			_cameraPos = cameraPos;
			_targetPos = targetPos;
			_slerpedDir = VNorm(VSub(cameraPos, targetPos));
			_initialize = true;
			return;
		}

		if (pInput->IsTiltingL())
		{
			// 入力中：現在の向きから目標の向きへ、球面線形補間で少しずつ近づける
			_nowDir = VNorm(VSub(cameraPos, targetPos));
			_slerpedDir = Slerp(_slerpedDir, _nowDir, CAMERA_SLERP_RATE);
		}
		else
		{
			// 無入力：向きは_slerpedDirのまま凍結する。
			// このとき、補間が追いつき切れていない分（遅れ）が残っているので、
			// 「実際に見えている向き」からyaw/pitchを逆算して生の値に書き戻し、遅れを清算する。
			// これにより、次の入力は必ず今見えている画角を開始地点として始まる。
			// ※_slerpedDirはCAMERA_DISTANCEが負のため全成分が反転している。
			// 　逆算時はマイナスを付けて反転を打ち消す
			_cameraPitch = std::asinf(-_slerpedDir.y);
			_cameraYaw = std::atan2f(-_slerpedDir.x, -_slerpedDir.z);

			// 逆算後の値もピッチの可動域内に収める
			_cameraPitch = std::clamp(_cameraPitch, -CAMERA_PITCH_LIMIT, -0.2f);
		}

		// カメラ位置は常に今の注視点を基準に組み立て直す（プレイヤーへの追従はここで保証される）
		_cameraPos = VAdd(targetPos, VScale(_slerpedDir, fabsf(CAMERA_DISTANCE)));
	}
	else
	{
		_cameraPos = cameraPos;
	}
	// 注視点を保存（描画時に使用）
	_targetPos = targetPos;
}

void Camera::Draw(int playerNum)
{
	switch (playerNum)
	{
	case 1:
		// 画面左半分をプレイヤー1用の描画範囲にする
		SetDrawArea(0, 0, WIDTH / 2, HEIGHT);
		// 消失点を左半分の中心に合わせる
		SetCameraScreenCenter(WIDTH / 4, HEIGHT);
		SetupCamera_Perspective(DX_PI_F / 4.0f);
		SetCameraPositionAndTargetAndUpVec(_cameraPos, _targetPos, VGet(0.0f, 1.0f, 0.0f));
		break;
	case 2:
		// 画面右半分をプレイヤー2用の描画範囲にする
		SetDrawArea(WIDTH / 2, 0, WIDTH, HEIGHT);
		// 消失点を右半分の中心に合わせる
		SetCameraScreenCenter((WIDTH * 3) / 4, HEIGHT);
		SetupCamera_Perspective(DX_PI_F / 4.0f);
		SetCameraPositionAndTargetAndUpVec(_cameraPos, _targetPos, VGet(0.0f, 1.0f, 0.0f));
		break;
	default:
		break;
	}

	DrawFormatString(3, 100, 0xffffff, "CameraMode : %d", _changedCameraMode);
}

VECTOR Camera::Slerp(VECTOR p0, VECTOR p1, float t)
{
	// 内積から、2つのベクトルがなす角のコサインを求める
	float cosTheta = VDot(p0, p1);
	// 浮動小数点誤差でacosの定義域を超えないよう、-1～1の間に収める
	cosTheta = std::clamp(cosTheta, -1.0f, 1.0f);
	// アークコサインで、コサインの値からなす角シータを求める
	double theta = std::acos(cosTheta);
	// サインの値を求める
	float sinTheta = std::sin(theta);
	// 処理切り替えの境界値
	const float eps = 1e-6;
	// サインの値が非常に小さい（＝2つのベクトルがほぼ同じ向きの）場合は、
	// ゼロ除算を避けるため通常の線形補間で処理を行う
	if (sinTheta < eps)
	{
		VECTOR lerp = VAdd(VScale(p0, 1 - t), VScale(p1, t));
		return lerp;
	}
	// 球面線形補間の公式における係数
	float coeff0 = std::sin((1.0f - t) * theta) / sinTheta;
	float coeff1 = std::sin(t * theta) / sinTheta;
	// 係数を使って最終的な向きを決定
	VECTOR slerp = VAdd(VScale(p0, coeff0), VScale(p1, coeff1));

	return slerp;
}