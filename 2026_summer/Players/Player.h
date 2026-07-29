#pragma once
#include "Bases//Character.h"
#include<memory>

class Input;
class PlayerMove;
class Camera;

class Player :
	public Character
{
public:
	Player();
	virtual ~Player() override;
	void Init() override;
	void End()override;
	void Update(std::shared_ptr<Input> pInput, std::shared_ptr<Camera> pCamera);
	void Draw() override;
	// プレイヤー座標のゲッター
	VECTOR GetPos() const { return _playerUnit.pos; }

private:
	// プレイヤー
	CharacterData _playerUnit;
	// 移動用クラスの共有ポインタ
	std::shared_ptr<PlayerMove> p_Move;
	// カメラの向いている角度
	float _cameraAngle;
	// モデルを向ける角度
	float _angle;
};