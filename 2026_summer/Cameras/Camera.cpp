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
	constexpr float DISTANCE = -1150.0f;
	// 水平方向の回転速度
	constexpr float YAW_SPEED = 0.03f;
	// 垂直方向の回転速度
	constexpr float PITCH_SPEED = 0.03f;
	// 線形補間度
	constexpr float LERP_RATE = 0.3f;
	// カメラの垂直方向の上限回転角度
	constexpr float PITCH_UP_LIMIT = -0.95f;
	// カメラの垂直方向の下限回転角度
	constexpr float PITCH_DOWN_LIMIT = -0.2f;
	// 入力値変換用の値
	constexpr float INPUT_COR = 0.001f;
	// プレイヤーとの距離
	constexpr float LOCKON_DISTANCE = 500.0f;
	// 横方向の補正値
	constexpr float LOCKON_RIGHT_OFFSET = 500.0f;
	// カメラの高さ
	constexpr float LOCKON_HEIGHT = 350.0f;
}

Camera::Camera() :
	_cameraYaw(0.0f),
	_cameraPitch(0.0f),
	_dispCameraYaw(0.0f),
	_dispCameraPitch(0.0f),
	_cameraPos(VGet(0.0f, 0.0f, 0.0f)),
	_targetPos(VGet(0.0f, 0.0f, 0.0f)),
	_cameraMode(true),
	_screenCenterY(HEIGHT * 5 / 6),
	_dirToEnemy(VGet(0.0f, 0.0f, 0.0f))
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

	// カメラモード切替
	if (pInput->IsTrigger(PAD_INPUT_C))
		_cameraMode = !_cameraMode;

	if (_cameraMode)
		NormalCam(rx, ry, pPlayer);
	else
		LockOnCam(pPlayer, pEnemy);

}

void Camera::Draw()
{
	// 消失点を画面下部に設定する
	SetCameraScreenCenter(WIDTH / 2, _screenCenterY);
	// カメラを設置
	SetCameraPositionAndTargetAndUpVec(_cameraPos, _targetPos, VGet(0.0f, 1.0f, 0.0f));
}

void Camera::NormalCam(int rx, int ry, std::shared_ptr<Player> pPlayer)
{
	_screenCenterY = HEIGHT * 5 / 6;

	// 注視点を設定
	if (pPlayer != nullptr) _targetPos = pPlayer->GetPos();

	// 入力値を-1.0～1.0の値に変換
	_cameraYaw += YAW_SPEED * std::clamp(rx * INPUT_COR, -1.0f, 1.0f);
	_cameraPitch += PITCH_SPEED * std::clamp(ry * INPUT_COR, -1.0f, 1.0f);

	// カメラの垂直方向の回転角度に制限をかける
	_cameraPitch = std::clamp(_cameraPitch, PITCH_UP_LIMIT, PITCH_DOWN_LIMIT);

	// 表示用の回転角度に線形補間をかける
	_dispCameraYaw += (_cameraYaw - _dispCameraYaw) * LERP_RATE;
	_dispCameraPitch += (_cameraPitch - _dispCameraPitch) * LERP_RATE;

	VECTOR cameraPos = VGet(0.0f, 0.0f, 0.0f);
	cameraPos.x = DISTANCE * std::cosf(_dispCameraPitch) * std::sinf(_dispCameraYaw);
	cameraPos.y = DISTANCE * std::sinf(_dispCameraPitch);
	cameraPos.z = DISTANCE * std::cosf(_dispCameraPitch) * std::cosf(_dispCameraYaw);

	_cameraPos = VAdd(cameraPos, _targetPos);
}

void Camera::LockOnCam(std::shared_ptr<Player>pPlayer, std::shared_ptr<Enemy>pEnemy)
{
	_screenCenterY = HEIGHT * 3 / 4;

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
	targetCamPos = VAdd(targetCamPos, VScale(_dirToEnemy, -LOCKON_DISTANCE));
	targetCamPos = VAdd(targetCamPos, VScale(right, LOCKON_RIGHT_OFFSET));
	targetCamPos.y = LOCKON_HEIGHT;

	// 注視点は敵にする
	VECTOR targetLookPos = VAdd(pEnemy->GetPos(), VGet(0.0f, TARGET_HEIGHT, 0.0f));


	// 線形補間をかける
	_cameraPos = VAdd(_cameraPos, VScale(VSub(targetCamPos, _cameraPos), LERP_RATE));
	_targetPos = VAdd(_targetPos, VScale(VSub(targetLookPos, _targetPos), LERP_RATE));
}