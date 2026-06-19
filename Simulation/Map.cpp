#include"Map.h"
#include"GameDefine.h"
#include"DxLib.h"
#include<sstream>
#include<fstream>
#include<string>

Map::Map() :
	_mapHandle(0)
{
	_mapHandle = LoadGraph("Data/matome.bmp");

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
	while (std::getline(file, line) && y < GameDefine::NODE_HEIGHT)
	{
		std::istringstream stream(line);
		std::string field;

		// ,　の区切りごとにデータを読み込む
		int x = 0;
		while (getline(stream, field, ',') && x < GameDefine::NODE_WIDTH)
		{
			_chipData[y][x] = std::stoi(field);
			x++;

		}
		y++;
	}
}

void Map::DrawMapChip()
{
	for (int y = 0; y < GameDefine::NODE_HEIGHT; y++)
	{
		for (int x = 0; x < GameDefine::NODE_WIDTH; x++)
		{
			Vec2 pos;
			pos.x = x * GameDefine::NODE_SIZE;
			pos.y = y * GameDefine::NODE_SIZE;

			if (pos.x < 0 - GameDefine::NODE_SIZE) continue;
			if (pos.x > GameDefine::WIDTH) continue;
			if (pos.y < 0 - GameDefine::NODE_SIZE) continue;
			if (pos.y > GameDefine::HEIGHT) continue;

			int chipNo = _chipData[y][x];

			Vec2 src;
			src.x = GameDefine::NODE_SIZE * (chipNo % 6);
			src.y = GameDefine::NODE_SIZE * (chipNo / 6);

			DrawRectRotaGraph(
				static_cast<int>(pos.x + GameDefine::NODE_SIZE * 0.5f),
				static_cast<int>(pos.y + GameDefine::NODE_SIZE * 0.5f),
				src.x, src.y,
				GameDefine::NODE_SIZE, GameDefine::NODE_SIZE,
				1.0f, 0.0f,
				_mapHandle, true
			);

			DrawLine(x * GameDefine::NODE_SIZE, 0, x * GameDefine::NODE_SIZE, GameDefine::HEIGHT, 0x000000, true);
			DrawLine(0, y * GameDefine::NODE_SIZE, GameDefine::WIDTH, y * GameDefine::NODE_SIZE, 0x000000, true);
		}
	}
}