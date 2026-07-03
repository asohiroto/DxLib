#include "ActorData.h"

namespace
{
	//Unity座標をDXライブラリに合わせる
	constexpr float kUnityToDXPosition = 100.0f;
	//要素数
	constexpr int kDataNum = 21;
}

ActorData::ActorData():
	m_actorID(0),
	m_actorType(ActorType::None),
	m_pos(Vector3::Zero()),
	m_rot(Vector3::Zero()),
	m_scale(Vector3::Zero()),
	m_modelPath(L"None"),
	m_priority(Priority::None),
	m_gameTag(GameTag::None),
	m_isTrough(true),
	m_isTrigger(false),
	m_isGravity(false),
	m_collRadius(0.0f),
	m_collHeight(0.0f),
	m_csvPathData(L"None")
{
}

ActorData::ActorData(std::shared_ptr<CSVData> data):
	m_actorID(0),
	m_actorType(ActorType::None),
	m_pos(Vector3::Zero()),
	m_rot(Vector3::Zero()),
	m_scale(Vector3::Zero()),
	m_modelPath(L"None"),
	m_priority(Priority::None),
	m_gameTag(GameTag::None),
	m_isTrough(true),
	m_isTrigger(false),
	m_isGravity(false),
	m_collRadius(0.0f),
	m_collHeight(0.0f),
	m_csvPathData(L"None")
{
	//データを取得
	this->m_data = data->GetData();
	//変換
	Conversion();
}

ActorData::~ActorData()
{
}

void ActorData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;
	//名前
	m_name = m_data[0];

	//ID
	m_actorID = std::stoi(m_data[1]);

	//アクタータイプ
	std::wstring tempStr = m_data[2];
	ActorType actorType = ActorType::None;
	if (tempStr == L"Character")actorType = ActorType::Character;
	else if (tempStr == L"Stage")actorType = ActorType::Stage;
	else if (tempStr == L"CheckPoint")actorType = ActorType::CheckPoint;
	else if (tempStr == L"Attack")actorType = ActorType::Attack;
	m_actorType = actorType;

	//座標
	m_pos.x = std::stof(m_data[3]) * kUnityToDXPosition;
	m_pos.y = std::stof(m_data[4]) * kUnityToDXPosition;
	m_pos.z = std::stof(m_data[5]) * kUnityToDXPosition;

	//回転
	m_rot.x = std::stof(m_data[6]);
	m_rot.y = std::stof(m_data[7]);
	m_rot.z = -1 * std::stof(m_data[8]);

	//大きさ
	m_scale.x = std::stof(m_data[9]);
	m_scale.y = std::stof(m_data[10]);
	m_scale.z = std::stof(m_data[11]);

	//モデルのパス
	m_modelPath = m_data[12];

	//優先度
	tempStr = m_data[13];
	Priority priority = Priority::None;
	if (tempStr == L"Low")priority = Priority::Low;
	else if (tempStr == L"Middle")priority = Priority::Middle;
	else if (tempStr == L"High")priority = Priority::High;
	else if (tempStr == L"Static")priority = Priority::Static;
	m_priority = priority;

	//ゲームタグ
	tempStr = m_data[14];
	GameTag gameTag = GameTag::None;
	if (tempStr == L"Player")gameTag = GameTag::Player;
	else if (tempStr == L"Enemy")gameTag = GameTag::Enemy;
	else if (tempStr == L"Item")gameTag = GameTag::Item;
	else if (tempStr == L"Object")gameTag = GameTag::Object;
	else if (tempStr == L"Field")gameTag = GameTag::Field;
	else if (tempStr == L"Attack")gameTag = GameTag::Attack;
	else if (tempStr == L"Area")gameTag = GameTag::Area;
	else if (tempStr == L"Sky")gameTag = GameTag::Sky;
	m_gameTag = gameTag;

	//当たり判定を無視するか
	m_isTrough = (m_data[15] == L"1");

	//トリガー
	m_isTrigger = (m_data[16] == L"1");

	//重力
	m_isGravity = (m_data[17] == L"1");

	//半径
	m_collRadius = std::stof(m_data[18]);

	//高さ
	m_collHeight = std::stof(m_data[19]);

	//パスまとめ
	m_csvPathData = m_data[20];
}