#include"RouteSearch.h"
#include"GameDefine.h"
#include"DxLib.h"
#include<queue>
#include<sstream>
#include<fstream>
#include"AsoDxLib/color.h"

using namespace GameDefine;

RouteSearch::RouteSearch() :
	_countTbl{},
	_moveCount(640),
	_funcCount(0),
	_frameCount(0),
	_fieldTbl()
{
	ClearCount();

	// csvファイルの読み込み
	std::ifstream file("Data/simulation_map.csv");
	// ファイル内の1行分のデータを入れる変数
	std::string line;

	int y = 0;
	// ファイルから1行分筒読み込んでいき、最大数に達するまで繰り返す
	while (std::getline(file, line) && y < NODE_HEIGHT)
	{
		// 先頭から参照できるようにする
		std::istringstream stream(line);
		// 実際の地形タイプを保存する変数
		std::string field;

		int x = 0;
		// 「,」による区切りごとに、1つのデータとして読み込んでいく
		while (getline(stream, field, ',') && x < NODE_WIDTH)
		{
			// fieldに格納された整数データをGetTileTypeを通して、地形データへと変換する
			_fieldTbl[y][x] = GetTileType(std::stoi(field));
			x++;
		}
		y++;
	}
}

RouteSearch::~RouteSearch()
{

}

void RouteSearch::Init()
{

}

void RouteSearch::Update()
{
	_frameCount++;

	_funcCount = 0;
}

void RouteSearch::Draw()
{
	int goalPosX = ENEMY_BASE_X * NODE_SIZE;
	int goalPosY = ENEMY_BASE_Y * NODE_SIZE;

	// ゴールの描画
	DrawBox(goalPosX, goalPosY, goalPosX + NODE_SIZE, goalPosY + NODE_SIZE, color::RedColor, true);

}

void RouteSearch::RouteSearchAstar(Vec2 startPos, int startCount, Vec2 goal)
{
	ClearCount();

	// 範囲外のチェック
	if (startPos.x < 0 || startPos.y < 0 || startPos.x >= NODE_WIDTH || startPos.y >= NODE_HEIGHT) return;

	// 開始地点のノード
	NodeData n;
	n.Pos = startPos;
	n.c = 0;
	n.h = abs(startPos.x - goal.x) + abs(startPos.y - goal.y);
	n.s = n.c + n.h;

	// sの値で並べ替えを行うキューを作成
	std::priority_queue<NodeData, std::vector<NodeData>, std::greater<NodeData>> que;

	// 開始地点をキューに格納し、開始地点の移動コストを初期化
	que.push(n);
	_countTbl[(int)startPos.y][(int)startPos.x] = startCount;

	// 探索するノードがなくなるまでループ
	while (!que.empty())
	{
		// 次に調べるノードを取り出し、削除
		NodeData curr = que.top();
		que.pop();

		// キューから取り出したノードがゴールなら探索終了
		if (curr.Pos.y == goal.y && curr.Pos.x == goal.x) return;

		// 既にここより効率の良いルートで探索済みのノードならスキップ
		int currentRemainning = startCount - curr.c;
		if (currentRemainning < _countTbl[static_cast<int>(curr.Pos.y)][static_cast<int>(curr.Pos.x)]) continue;

		// カウント用
		_funcCount++;

		// 探索の方向設定用
		Vec2 dir[] = { Vec2(1,0) , Vec2(-1, 0),Vec2(0, 1),Vec2(0,-1) };

		for (int i = 0; i < 4; i++)
		{
			// 次のチェック情報
			Vec2 o = curr.Pos + dir[i];

			// 範囲外チェック
			if (o.x < 0 || o.x >= NODE_WIDTH || o.y < 0 || o.y >= NODE_HEIGHT) continue;

			// 必要な移動コストを算出
			int moveCost = GetMoveCost(_fieldTbl[(int)o.y][(int)o.x]);

			// 残りの移動コストを、実コストと地形コストから計算
			int nc = startCount - (curr.c + moveCost);

			// 移動力と、まだ探索していないかのチェック
			if (moveCost < 0 || nc < 0) continue;
			if (_countTbl[(int)o.y][(int)o.x] >= 0) continue;

			// テーブルにスコアを保存
			_countTbl[(int)o.y][(int)o.x] = nc;

			// 調べる予定のノードを設定し、キューに入れる
			NodeData d;
			d.Pos = o;
			d.c = curr.c + moveCost;
			d.h = abs(o.x - goal.x) + abs(o.y - goal.y);
			d.s = d.c + d.h;

			que.push(d);

		}
	}
}

int RouteSearch::GetMoveCost(TileType type)
{
	switch (type)
	{
	case TileType::Plain: return 1;
	case TileType::MyBase: return 1;
	case TileType::River: return 3;
	case TileType::Forest: return 2;
	case TileType::EnemyBase: return 2;
	case TileType::Mountain: return 2;
	default: return 999;
	}
}

TileType RouteSearch::GetTileType(int num)
{
	switch (num)
	{
	case 0: return TileType::Plain;
	case 1: return TileType::River;
	case 2: return TileType::MyBase;
	case 3: return TileType::Forest;
	case 4: return TileType::EnemyBase;
	case 5: return TileType::Mountain;
	}
}

std::vector<Vec2> RouteSearch::GetRouteList(Vec2 startPos, Vec2 goal)
{
	std::vector<Vec2> route;
	// ゴールまでの歩数がわからなければ、終了
	if (_countTbl[(int)goal.y][(int)goal.x] < 0) return route;

	Vec2 index = goal;

	route.push_back(index);

	while (1)
	{
		// ひとつ前に探索した時の移動コスト
		int currentRemaining = _countTbl[(int)index.y][(int)index.x];
		// 上下左右の移動用ベクトル
		Vec2 dir[] = { Vec2(1, 0), Vec2(-1, 0), Vec2(0,1), Vec2(0,-1) };
		// 次を探索するかのフラグ
		bool foundNext = false;
		// 上下左右の４回分のループ
		for (int i = 0; i < 4; i++)
		{
			Vec2 next = index + dir[i];
			// 範囲外なら探索しない
			if (next.x < 0 || next.x >= NODE_WIDTH || next.y < 0 || next.y >= NODE_HEIGHT) continue;
			// 地形情報による移動コスト
			int cost = GetMoveCost(_fieldTbl[(int)index.y][(int)index.x]);
			// 最短経路の確認
			if (_countTbl[(int)next.y][(int)next.x] == currentRemaining + cost)
			{
				index = next;

				route.push_back(index);

				foundNext = true;
				break;
			}
		}
		// スタート地点まで戻ったら終了
		if (index.x == startPos.x && index.y == startPos.y) break;
		if (!foundNext) break;
	}
	// ルートを逆順にする
	std::reverse(route.begin(), route.end());

	return route;
}

void RouteSearch::ClearCount()
{
	// 全てのノードの移動コストを最小の値で初期化する
	for (int x = 0; x < NODE_WIDTH; x++)
	{
		for (int y = 0; y < NODE_HEIGHT; y++)
		{
			_countTbl[y][x] = -1;
		}
	}
}

void RouteSearch::DrawRoute(Vec2 goal)
{
	// ゴールまでの歩数がわからなければ、終了
	if (_countTbl[(int)goal.y][(int)goal.x] < 0) return;

	// 現在チェックしているノードの番号（座標ではない）
	Vec2 index = goal;

	while (1)
	{
		// indexをスクリーン上の座標に変換し、描画
		Vec2 pos = index * NODE_SIZE;
		DrawBox(pos.x, pos.y, pos.x + NODE_SIZE, pos.y + NODE_SIZE, color::YellowColor, true);

		// ひとつ前に探索した時の移動コスト
		int currentRemaining = _countTbl[(int)index.y][(int)index.x];

		// 上下左右の移動用ベクトルと、次を探索するかのフラグ
		Vec2 dir[] = { Vec2(1,0), Vec2(-1,0), Vec2(0,1), Vec2(0, -1) };
		bool foundNext = false;

		// 上下左右の４回分のループ
		for (int i = 0; i < 4; i++)
		{
			// 今から調べる上下左右いずれかのノード
			Vec2 next = index + dir[i];

			// 範囲外なら探索しない
			if (next.x < 0 || next.x >= NODE_WIDTH || next.y < 0 || next.y >= NODE_HEIGHT) continue;

			// 地形情報による移動コスト
			int cost = GetMoveCost(_fieldTbl[(int)index.y][(int)index.x]);

			// 最短経路の確認
			if (_countTbl[(int)next.y][(int)next.x] == currentRemaining + cost)
			{
				index = next;
				foundNext = true;
				break;
			}
		}
		if (!foundNext) break;
	}
}
