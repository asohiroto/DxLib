#include"Map.h"
#include"GameDefine.h"
#include"DxLib.h"
#include<sstream>
#include<fstream>
#include<string>

using namespace GameDefine;

Map::Map() :
	_mapHandle(0)
{
	_mapHandle = LoadGraph("Data/マップ素材.png");

	LoadMapData();
}

Map::~Map()
{

}

void Map::Init()
{

}

void Map::Update()
{

}

void Map::Draw()
{
	DrawMapChip();
}

void Map::LoadMapData()
{
	std::ifstream file("Data/simulation_map.csv");
	std::string line;

	// getline関数で1行ずつ読み込む
	int y = 0;
	while (std::getline(file, line) && y < NODE_HEIGHT)
	{
		std::istringstream stream(line);
		std::string field;

		// ,　の区切りごとにデータを読み込む
		int x = 0;
		while (getline(stream, field, ',') && x < NODE_WIDTH)
		{
			_chipData[y][x] = std::stoi(field);
			x++;
		}
		y++;
	}
}

void Map::DrawMapChip()
{
	for (int y = 0; y < NODE_HEIGHT; y++)
	{
		for (int x = 0; x < NODE_WIDTH; x++)
		{
			Vec2 pos;
			pos.x = x * NODE_SIZE;
			pos.y = y * NODE_SIZE;

			// 画面外のチップは描画しない
			if (pos.x < 0 - NODE_SIZE) continue;
			if (pos.x > WIDTH) continue;
			if (pos.y < 0 - NODE_SIZE) continue;
			if (pos.y > HEIGHT) continue;

			int chipNo = _chipData[y][x];

			// チップ番号から描画する画像の座標を計算
			Vec2 src;
			src.x = NODE_SIZE * (chipNo % 6);
			src.y = 0;

			// 画像を描画
			DrawRectRotaGraph(
				static_cast<int>(pos.x + NODE_SIZE * 0.5f), static_cast<int>(pos.y + NODE_SIZE * 0.5f),
				src.x, src.y, NODE_SIZE, NODE_SIZE,
				1.0f, 0.0f,
				_mapHandle, true
			);

			DrawLine(x * NODE_SIZE, 0, x * NODE_SIZE, (y + 1) * NODE_SIZE, 0x000000, true);
			DrawLine(0, y * NODE_SIZE, (x + 1) * NODE_SIZE, y * NODE_SIZE, 0x000000, true);
		}
	}
}