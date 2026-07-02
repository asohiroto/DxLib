#pragma once
#include"GameDefine.h"
#include<vector>
#include"AsoDxLib/Vec2.h"

struct NodeData
{
	// 位置
	Vec2 Pos;
	/// <summary>
	/// 地形情報
	/// 平地 ０,
	/// 自軍拠点 １,
	/// 河川 ２,
	/// 森林 ３,
	/// 敵軍拠点 ４,
	/// 山 ５
	/// </summary>
	TileType type;
	// 実コスト　開始地点から現在までの移動歩数
	int c;
	// 推定コスト　マンハッタン距離によるゴールまでの推定コスト
	int h;
	// スコア　実コスト+推定コスト
	int s;
	// 演算子オーバーロード　ノードを＞で比較した場合はｓの値で比較するようにする
	bool operator>(const NodeData& other) const { return s > other.s; }
};

class RouteSearch
{
public:
	RouteSearch();
	~RouteSearch();
	void Init();
	void Update();
	void Draw();
	

	// A*による経路探索 (_countTblにスコアを書き込んでいく)
	void RouteSearchAstar(Vec2 startPos, int startCount, Vec2 goal);
	// 探査した経路を保存する関数 (_countTblを参照してルートを決定する)
	std::vector<Vec2> GetRouteList(Vec2 startPos, Vec2 goal);
	// ゴールまでの道筋を表示
	void DrawRoute(Vec2 goal);
	// ノード座標を受け取って地形を返す関数
	TileType GetNodeData(int nodeX, int nodeY) { return _fieldTbl[nodeY][nodeX]; }
	// TileType型を受け取って移動コストを返す関数
	int GetMoveCost(TileType type);

public:
	// 最大探索コストの上限値（この値まで探索を行える）
	int _moveCount;
	// 地形情報（移動コスト）
	TileType _fieldTbl[GameDefine::NODE_HEIGHT][GameDefine::NODE_WIDTH];

private:
	// 地形情報のクリア
	void ClearCount();
	// 数字を受け取って、TileTypeを返す関数
	TileType GetTileType(int num);

private:
	// 各マス時点での残りの移動コストを記録する（最大を保存）
	int _countTbl[GameDefine::NODE_HEIGHT][GameDefine::NODE_WIDTH];
	// 再帰関数が呼び出された回数
	int _funcCount;
	// フレーム数のカウント
	int _frameCount;
};