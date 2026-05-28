#pragma once
#include"Character.h"

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public Character
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init() override;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

private:
	int m_handleFlag;
	int m_x, m_y;

	bool flagFlag;
};