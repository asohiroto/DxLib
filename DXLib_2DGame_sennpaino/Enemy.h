#pragma once
#include"Character.h"
#include"Player.h"
#include"Castle.h"

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
	virtual void Init(int type);

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update(Player* pPlayer, Castle* pCastle);

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

protected:

private:
	Player* m_pPlayer;
	Castle* m_pCastle;
};