#pragma once
#include"Character.h"
#include"Player.h"

/// <summary>
/// 敵クラス
/// </summary>
class Enemy : public Character
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Enemy();

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Enemy();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init(Player* pPlayer);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

protected:

private:
	Player* m_pPlayer;

	
};