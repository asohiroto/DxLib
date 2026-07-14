#include"Map.h"
#include"GameDefine.h"
#include"AsoDxLib/Color.h"
#include"DxLib.h"
#include<sstream>
#include<fstream>
#include<string>

using namespace GameDefine;

Map::Map() :
	_map2H(-1),
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
	_map2H = LoadGraph("data/GameBG.png");
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

	DrawGraph(0, 0, _map2H, true);

	for (int x = 0; x < NODE_WIDTH; x++)
	{
		int px = x * NODE_SIZE;
		if (-NODE_SIZE >= px || px >= WIDTH) continue;
		DrawLine(px, 0, px, HEIGHT, color::DarkGrayColor, 1);
	}

	for (int y = 0; y < NODE_HEIGHT; y++)
	{
		int py = y * NODE_SIZE;
		if (-NODE_SIZE >= py || py >= HEIGHT) continue;
		DrawLine(0, py, WIDTH, py, color::DarkGrayColor, 1);
	}

	DrawRectGraph(MY_BASE_X * NODE_SIZE, MY_BASE_Y * NODE_SIZE, 50, 0, 25, 25, _mapHandle, true);
	DrawRectGraph(ENEMY_BASE_X * NODE_SIZE, ENEMY_BASE_Y * NODE_SIZE, 100, 0, 25, 25, _mapHandle, true);
}