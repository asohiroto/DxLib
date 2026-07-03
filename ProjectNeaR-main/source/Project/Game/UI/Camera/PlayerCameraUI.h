#pragma once
#include "../UIBase.h"
#include "../../../General/Math/MyMath.h"
#include <memory>
class EnemyBase;
class PlayerCamera;
class PlayerCameraUI :
    public UIBase
{
public:
	PlayerCameraUI(std::weak_ptr<PlayerCamera> pCamera);
	~PlayerCameraUI();
	//更新
	void Update() override;
	//描画
	void Draw()const override;

	//ロックオンか
	bool IsLockOn()const { return m_isLockOn; };
	//有効
	void EnableLockOn(std::weak_ptr<EnemyBase> target);
	//無効
	void DisableLockOn();
private:
	//表示
	bool m_isLockOn;

	//ターゲット
	std::weak_ptr<EnemyBase> m_pTarget;

	//カメラ
	std::weak_ptr<PlayerCamera> m_pCamera;

	//バーの構造体
	struct LockOnUI
	{
		int handle = -1;
		Vector2 pos;
		double angle = 0.0f;
	};
	LockOnUI m_leftVar;
	LockOnUI m_rightVar;
	//ロックオンサークル
	LockOnUI m_lockOnCircle;

	//1フレーム前のカメラの向き
	Vector3 m_beforeCameraDir;
	
};

