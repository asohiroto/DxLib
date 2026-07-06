#pragma once
#include"DxLib.h"

class Player
{
public:
	Player();
	virtual ~Player();
	void Init();
	// プレイヤーの動き
	// カメラのアングルを引数とする
	void Update(float cameraAngle);
	void Draw();
	// プレイヤーの座標のゲッター
	VECTOR GetPos() const { return _pos; }

private:
	// キャラクターが向いている方向
	float _characterAngle;
	// モデルのハンドル
	int _handle;
	// モデルの座標
	VECTOR _pos;
	// キャラクターがどこまで動いたか
	VECTOR _move;
	// カメラの開店を方向に変換した値を保存する
	VECTOR _movementDirection;
	// カメラのマトリックス
	MATRIX _rotMatrix;
	// モデルの回転行列を作成
	MATRIX _modelRotMatrix;
};

