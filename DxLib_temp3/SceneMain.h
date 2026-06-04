#pragma once
#include "DXLib.h"

class SceneMain
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneMain();

	/// <summary>
	///  デストラクタ
	/// </summary>
	~SceneMain();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:

	// 表示するグリッドの数
	static constexpr int kGridNumX = 16;
	static constexpr int kGridNumY = 9;
	static constexpr int kGridSize = 100;

	// 再帰
	int m_moveCount;
	int m_countTbl[kGridNumY][kGridNumX];

	// 再帰関数が呼び出された回数
	int m_saikiCount;

	// フィールドの状態を保存するテーブル
	int m_fieldTbl[kGridNumY][kGridNumX];

	// ゴールとスタートの位置
	int m_goalX;
	int m_goalY;
	int m_startX;
	int m_startY;
};