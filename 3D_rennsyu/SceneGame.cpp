#include "SceneGame.h"
#include"Player.h"
#include"Camera.h"
#include"GameObject.h"
#include"Ground.h"
#include<DxLib.h>
#include<fstream>
#include<sstream>
#include<string>

namespace
{
	constexpr int kGridNum = 10;
	constexpr int kGridSize = 100;
	constexpr int kElementNum = 0;
	const char* kFilePath = "data/GameObjectData.csv";
}

SceneGame::SceneGame()
{

}

SceneGame::~SceneGame()
{

}

void SceneGame::Init()
{
	p_Player = std::make_shared<Player>();
	p_Player->Init();

	auto& camera = Camera::GetInstance();
	camera.Init();

	LoadGameObject();
}

void SceneGame::Update()
{

	SetCameraPositionAndAngle(VGet(0, 500, -1000), 0.5f, 0, 0);
	p_Player->Update();

	for (auto& object : p_GameObjects)
	{
		object->Update();
	}

	auto& camera = Camera::GetInstance();
	camera.Update(p_Player);

	std::vector<std::shared_ptr<GameObject>> collidedObjects;
	collidedObjects = p_GameObjects;
	collidedObjects.push_back(p_Player);

	for (auto& objA : collidedObjects)
	{
		if (!objA->IsCollidable()) continue;

		objA->SetGrounded(false);

		for (auto& objB : collidedObjects)
		{
			if (objA == objB) continue;

			GameObject::CollisionType typeA = objA->GetCollisionType();
			GameObject::CollisionType typeB = objB->GetCollisionType();

			if (typeA == GameObject::CollisionType::Poly
				&& typeB == GameObject::CollisionType::Poly) continue;

			if (typeA == GameObject::CollisionType::Sphere)
			{
				if (typeB == GameObject::CollisionType::Sphere)
				{
					float distance = VSize(VSub(objA->GetPos(), objB->GetPos()));

					if (distance < objA->GetRadius() + objB->GetRadius())
					{

					}
				}
				else if (typeB == GameObject::CollisionType::Poly)
				{
					VECTOR pos = objA->GetPos();
					float radius = objA->GetRadius();
					int polyH = objB->GetPolyH();

					auto result = MV1CollCheck_Sphere(polyH, -1, pos, radius, -1);

					if (result.HitNum > 0)
					{
						VECTOR totalPush = VGet(0.0f, 0.0f, 0.0f);
						bool grounded = false;

						for (int i = 0; i < result.HitNum; i++)
						{
							VECTOR norm = result.Dim[i].Normal;
							VECTOR planePoint = result.Dim[i].Position[0];

							float distToPlane = VDot(VSub(pos, planePoint), norm);
							float pushAmount = (radius - distToPlane) + 0.01f;

							if (pushAmount > 0.0f)
							{
								totalPush = VAdd(totalPush, VScale(norm, pushAmount));

								if (norm.y > 0.5f) // 上向きの面だけを「床」とみなす
								{
									grounded = true;
								}
							}
						}

						objA->SetPos(VAdd(objA->GetPos(), totalPush));

						if (grounded)
						{
							VECTOR vel = objA->GetVec();
							vel.y = 0.0f;
							objA->SetVec(vel);
							objA->SetGrounded(true);
						}
					}

					MV1CollResultPolyDimTerminate(result);
				}
			}
		}
	}
}

void SceneGame::Draw() const
{
	DrawGrid();
	p_Player->Draw();

	for (auto& object : p_GameObjects)
	{
		object->Draw();
	}
}

void SceneGame::DrawGrid() const
{
	const float lineStartX = kGridSize * -(kGridNum * 0.5f);
	const float lineEndX = -lineStartX;

	for (int z = 0; z <= kGridNum; z++)
	{
		const float lineZ = kGridSize * z - kGridSize * kGridNum * 0.5f;
		DrawLine3D(VGet(lineStartX, 0, lineZ), VGet(lineEndX, 0, lineZ), 0xffffff);
	}

	const float lineStartZ = kGridSize * -(kGridNum * 0.5f);
	const float lineEndZ = -lineStartZ;

	for (int x = 0; x <= kGridNum; x++)
	{
		const float lineX = kGridSize * x - kGridSize * kGridNum * 0.5f;
		DrawLine3D(VGet(lineX, 0, lineStartZ), VGet(lineX, 0, lineEndZ), 0xffffff);
	}
}

void SceneGame::LoadGameObject()
{
	std::ifstream file(kFilePath);

	if (!file.is_open()) return;

	std::string line;
	std::vector<std::vector<std::string>> valuesDatas;

	bool isHeader = true;

	while (std::getline(file, line))
	{
		if (isHeader)
		{
			isHeader = false;
			continue;
		}

		std::stringstream ss(line);
		std::string part;
		std::vector<std::string> values;

		while (std::getline(ss, part, ','))
		{
			values.emplace_back(part);
		}

		if (values.size() < kElementNum) continue;

		valuesDatas.emplace_back(values);
	}

	std::string name;
	VECTOR pos;
	VECTOR rot;
	VECTOR scale;

	for (auto& values : valuesDatas)
	{
		name = values[0];

		pos.x = std::stof(values[1]);
		pos.y = std::stof(values[2]);
		pos.z = std::stof(values[3]);

		rot.x = std::stof(values[4]);
		rot.y = std::stof(values[5]);
		rot.z = std::stof(values[6]);

		scale.x = std::stof(values[7]);
		scale.y = std::stof(values[8]);
		scale.z = std::stof(values[9]);

		if (name == "Ground")
		{
			std::shared_ptr<Ground> ground = std::make_shared<Ground>();
			ground->Init();
			ground->SetPos(pos);
			ground->SetRot(rot);
			ground->SetScale(scale);
			p_GameObjects.emplace_back(ground);
			continue;
		}
	}
}