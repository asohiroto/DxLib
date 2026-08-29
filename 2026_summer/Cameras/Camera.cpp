#include "Camera.h"
#include "Players/Player.h"
#include "Inputs/Input.h"
#include "GameDefine.h"
#include "Enemys/Enemy.h"
#include <algorithm>
#include <cmath>

using namespace GameDefine;

namespace
{
	// 注視点の補正
	constexpr float TARGET_HEIGHT = 50.0f;
	// プレイヤーまでの距離
	constexpr float DISTANCE = 550.0f;
	// 水平方向の回転速度
	constexpr float YAW_SPEED = 0.03f;
	// 線形補間度
	constexpr float LERP_RATE = 0.3f;
	// 入力値変換用の値
	constexpr float INPUT_OFFSET = 0.001f;
	// 横方向の補正値
	constexpr float RIGHT_OFFSET = 350.0f;
	// カメラの高さ
	constexpr float LOCKON_HEIGHT = 350.0f;
	// カメラの垂直方向の角度
	constexpr float CAMERA_PITCH = -0.8f;
	// 注視点までの距離
	constexpr float TARGET_DISTANCE = 15000000.0f;
}

Camera::Camera() :
	_cameraYaw(0.0f),
	_dispCameraYaw(0.0f),
	_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	_targetPos(VGet(0.0f, 0.0f, 0.0f)),
	_cameraMode(true),
	_dirToEnemy(VGet(0.0f, 0.0f, 0.0f)),
	_cameraCount(0)
{
}

Camera::~Camera()
{
}

void Camera::Init()
{
}

void Camera::End()
{
}

void Camera::Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy>pEnemy, std::shared_ptr<Input> pInput)
{
	// 右スティックの入力を保存
	int rx = pInput->GetRightStickX();
	int ry = -(pInput->GetRightStickY());

	_cameraCount++;

	if (_cameraCount == 1)
		_cameraMode = false;

	if (_cameraMode)
		LockOnCam(pPlayer, pEnemy);
	else
		NormalCam(pPlayer);
}

void Camera::Draw()
{
	// 消失点を画面下部に設定する
	SetCameraScreenCenter(WIDTH / 2, SCREEN_CENTER_Y);
	// カメラを設置
	SetCameraPositionAndTargetAndUpVec(_cameraPos, _targetPos, VGet(0.0f, 1.0f, 0.0f));

	SetupCamera_Perspective(DX_PI_F / 2);
}

void Camera::NormalCam(std::shared_ptr<Player> pPlayer)
{
	if (pPlayer == nullptr) return;

	// GetPlayerAngle()はモデル表示用に+DX_PI_Fされた角度なので、補正を外して実際の向きに戻す
	_cameraYaw = pPlayer->GetPlayerAngle();

	float sinYaw = std::sinf(_cameraYaw);
	float cosYaw = std::cosf(_cameraYaw);

	// プレイヤーの正面方向と右方向
	VECTOR front = VGet(sinYaw, 0.0f, cosYaw);
	VECTOR right = VGet(cosYaw, 0.0f, -sinYaw);

	// カメラの目標位置:プレイヤーの後ろ+右+高さ
	VECTOR targetCamPos = pPlayer->GetPos();
	targetCamPos = VAdd(targetCamPos, VScale(front, -DISTANCE * std::cosf(CAMERA_PITCH)));
	targetCamPos = VAdd(targetCamPos, VScale(right, RIGHT_OFFSET));
	targetCamPos.y += DISTANCE * -std::sinf(CAMERA_PITCH);

	// 注視点:プレイヤーの正面方向の先
	VECTOR targetLookPos = VAdd(pPlayer->GetPos(), VScale(front, TARGET_DISTANCE));
	targetLookPos.y += TARGET_HEIGHT;

	// LockOnCamと同じ方式で位置に線形補間をかける
	_cameraPos = VAdd(_cameraPos, VScale(VSub(targetCamPos, _cameraPos), LERP_RATE));
	_targetPos = VAdd(_targetPos, VScale(VSub(targetLookPos, _targetPos), LERP_RATE));
}

void Camera::LockOnCam(std::shared_ptr<Player>pPlayer, std::shared_ptr<Enemy>pEnemy)
{
	// 注視点を設定
	if (pEnemy == nullptr || pPlayer == nullptr) return;

	// 敵までの方向（水平成分のみ）
	_dirToEnemy = VSub(pEnemy->GetPos(), pPlayer->GetPos());
	_dirToEnemy.y = 0.0f;
	_dirToEnemy = VNorm(_dirToEnemy);

	_cameraYaw = atan2f(_dirToEnemy.x, _dirToEnemy.z);

	// 右側補正用のベクトル
	VECTOR right = VGet(_dirToEnemy.z, 0.0f, -_dirToEnemy.x);

	// カメラの位置設定
	VECTOR targetCamPos = pPlayer->GetPos();
	targetCamPos = VAdd(targetCamPos, VScale(_dirToEnemy, -DISTANCE));
	targetCamPos = VAdd(targetCamPos, VScale(right, RIGHT_OFFSET));
	targetCamPos.y = LOCKON_HEIGHT;

	// 注視点は敵にする
	VECTOR targetLookPos = VAdd(pEnemy->GetPos(), VGet(0.0f, TARGET_HEIGHT, 0.0f));

	// 線形補間をかける
	_cameraPos = VAdd(_cameraPos, VScale(VSub(targetCamPos, _cameraPos), LERP_RATE));
	_targetPos = VAdd(_targetPos, VScale(VSub(targetLookPos, _targetPos), LERP_RATE));
}