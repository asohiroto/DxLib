#include "AttackData.h"
#include "../StringUtil.h"

namespace
{
	constexpr int kDataNum = 29;
}

AttackData::AttackData():
	m_name(),
	m_isMultipleHit(false),
	m_attackType(AttackData::AttackType::None),
	m_attackWeight(CharaStatus::AttackWeight::Light),
	m_armor(CharaStatus::Armor::Light),
	m_cancelFrame(0),
	m_attackPower(0),
	m_knockBackPower(0.0f),
	m_verticalPower(0.0f),
	m_startFrame(0),
	m_keepFrame(0),
	m_nextAttackName(),
	m_radius(0.0f),
	m_length(0.0f),
	m_hitStopFrame(0),
	m_hitStopShakePower(0),
	m_param1(0.0f),
	m_param2(0.0f),
	m_param3(0.0f),
	m_param4(0.0f),
	m_moveSpeed(0.0f),
	m_moveFrame(0),
	m_attackOriginPosType(AttackOriginPosType::Owner),
	m_hitEffectPath(L""),
	m_attackEffectPath(L""),
	m_seAppearPath(L""),
	m_seHitPath(L""),
	m_voicePath(L"")
{
}

AttackData::AttackData(std::shared_ptr<CSVData> data):
	m_name(),
	m_isMultipleHit(false),
	m_attackType(AttackData::AttackType::None),
	m_attackWeight(CharaStatus::AttackWeight::Light),
	m_armor(CharaStatus::Armor::Light),
	m_cancelFrame(0),
	m_attackPower(0),
	m_knockBackPower(0.0f),
	m_verticalPower(0.0f),
	m_startFrame(0),
	m_keepFrame(0),
	m_nextAttackName(),
	m_radius(0.0f),
	m_length(0.0f),
	m_hitStopFrame(0),
	m_hitStopShakePower(0),
	m_param1(0.0f),
	m_param2(0.0f),
	m_param3(0.0f),
	m_param4(0.0f),
	m_attackOriginPosType(AttackOriginPosType::Owner),
	m_hitEffectPath(L""),
	m_attackEffectPath(L""),
	m_seAppearPath(L""),
	m_seHitPath(L""),
	m_voicePath(L"")
{
	//データを取得
	this->m_data = data->GetData();
	Conversion();
}

AttackData::~AttackData()
{
}

void AttackData::Conversion()
{
	//要素が足りないなら早期リターン
	if (m_data.size() != kDataNum)return;

	//攻撃の名前
	m_name = m_data[0];

	//ヒット番号
	m_isMultipleHit = m_data[1] == L"TRUE";

	//攻撃タイプ
	m_attackType = static_cast<AttackType>(stoi(m_data[2]));

	//威力
	m_attackPower = stoi(m_data[3]);

	//攻撃の重さ
	m_attackWeight = static_cast<CharaStatus::AttackWeight>(stoi(m_data[4]));

	//アーマー
	m_armor = static_cast<CharaStatus::Armor>(stoi(m_data[5]));

	//ノックバックの大きさ
	m_knockBackPower = stof(m_data[6]);

	//上方向の力
	m_verticalPower = stof(m_data[7]);

	//発生フレーム
	m_startFrame = stoi(m_data[8]);

	//持続フレーム
	m_keepFrame = stoi(m_data[9]);

	//半径
	m_radius = stof(m_data[10]);

	//長さ
	m_length = stof(m_data[11]);

	//前進速度
	m_moveSpeed = stof(m_data[12]);

	//前進フレーム
	m_moveFrame = stoi(m_data[13]);

	//アニメーションの名前
	m_animName = m_data[14];

	//次の攻撃の名前
	m_nextAttackName = m_data[15];

	//攻撃の発生位置
	AttackOriginPosType oriPos = AttackOriginPosType::Owner;
	if (m_data[16] == L"Owner")oriPos = AttackOriginPosType::Owner;
	else if (m_data[16] == L"Center")oriPos = AttackOriginPosType::Center;

	m_attackOriginPosType = oriPos;

	//キャンセルフレーム
	m_cancelFrame = stoi(m_data[17]);

	//ヒットストップのフレーム
	m_hitStopFrame = stoi(m_data[18]);

	//ヒットストップの揺れ
	m_hitStopShakePower = stoi(m_data[19]);

	//ヒットエフェクトパス
	m_hitEffectPath = m_data[20];

	//攻撃エフェクトパス
	m_attackEffectPath = m_data[21];

	//攻撃発生SE
	m_seAppearPath = m_data[22];

	//攻撃ヒットSE
	m_seHitPath = m_data[23];

	//攻撃ボイス
	m_voicePath = m_data[24];

	//パラメータ
	m_param1 = stof(m_data[25]);
	m_param2 = stof(m_data[26]);
	m_param3 = stof(m_data[27]);
	m_param4 = stof(m_data[28]);
}
