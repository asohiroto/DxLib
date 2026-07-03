#include "CharacterBase.h"
#include "../../../General/CharaStatus.h"
#include "../../../General/Model.h"
#include "../ActorManager.h"
#include "../../Attack/AttackManager.h"
#include "../../../General/CSV/CSVDataLoader.h"
#include "../../../General/CSV/CSVData.h"
#include "../../../General/CSV/AttackData.h"
#include "../../../General/CSV/EffectData.h"
#include "../../../General/CSV/SoundData.h"
#include "../../../General/AssetManager.h"
#include "../../../General/Collision/CapsuleCollider.h"
#include "../../../General/Collision/Rigidbody.h"
#include "../../../General/Sound/SoundManager.h"
#include "CharacterStateBase.h"
#include <cassert>
CharacterBase::CharacterBase(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData, Shape shape, std::weak_ptr<ActorManager> pActorManager) :
	Actor(actorData,shape,pActorManager),
	m_state(),
	m_charaStatus()
{
	m_charaStatus = std::make_shared<CharaStatus>(charaStatusData);
}

void CharacterBase::Init(std::wstring animPath, std::wstring attackPath, std::wstring effectPath, std::wstring sePath, std::wstring voicePath)
{
	//Physics‚É“o˜^
	Collidable::Init();
	//CSV‚ð“Ç‚Ýž‚Þ
	auto& csvLoader = CSVDataLoader::GetInstance();
	if (animPath != L"" && animPath != L"None")
	{
		//ƒAƒjƒ[ƒVƒ‡ƒ“ƒf[ƒ^
		InitAnimData(csvLoader, animPath);
	}
	if (attackPath != L"" && attackPath != L"None")
	{
		//UŒ‚ƒf[ƒ^
		InitAttackData(csvLoader, attackPath);
	}
	if (effectPath != L"" && effectPath != L"None")
	{
		//ƒGƒtƒFƒNƒg‚Ì€”õ
		InitEffectData(csvLoader, effectPath);
	}
	if (sePath != L"" && sePath != L"None")
	{
		//SE‚Ì€”õ
		InitSE(csvLoader, sePath);
	}
	if (voicePath != L"" && voicePath != L"None")
	{
		//Voice‚Ì€”õ
		InitVoice(csvLoader, voicePath);
	}
}


void CharacterBase::Update()
{
	//ó‘Ô‚É‡‚í‚¹‚½XV
	m_state->Update();
	//ó‘Ô‚ª•Ï‚í‚Á‚½‚©‚ðƒ`ƒFƒbƒN
	if (m_state != m_state->GetNextState())
	{
		//ó‘Ô‚ð•Ï‰»‚·‚é
		m_state = m_state->GetNextState();
		m_state->Init();
	}
	//ƒAƒjƒ[ƒVƒ‡ƒ“‚ÌXV
	m_model->Update();

	//ó‘Ô‚ÌƒŠƒZƒbƒg
	m_charaStatus->InitHitState();
}

std::shared_ptr<CharaStatus> CharacterBase::GetCharaStatus() const
{
	std::shared_ptr<CharaStatus> status;
	if (m_charaStatus)
	{
		status = m_charaStatus;
	}
	return status;
}

void CharacterBase::SetAttack(std::shared_ptr<AttackBase> attack)
{
	if (m_pActorManager.expired())return;
	auto actorManager = m_pActorManager.lock();
	//UŒ‚ƒ}ƒl[ƒWƒƒ[
	if (actorManager->GetAttackManager().expired())return;
	auto attackManager = actorManager->GetAttackManager().lock();

	attackManager->Entry(attack);
}


std::shared_ptr<AttackData> CharacterBase::GetAttackData(std::wstring attackName) const
{
	std::shared_ptr<AttackData> attackData;

	//’T‚·
	for (auto& data : m_attackDatas)
	{
		//ðŒ‚É‡‚¤‚à‚Ì‚ª‚ ‚Á‚½‚ç
		if (data->GetName() == attackName)
		{
			attackData = data;
			break;
		}
	}

	assert(attackData);

	return attackData;
}

std::string CharacterBase::GetAnim(std::wstring state, std::string path,AnimData::WeaponType type)const
{
	//’T‚·
	for (auto& data : m_animDatas)
	{
		//ðŒ‚É‡‚¤‚à‚Ì‚ª‚ ‚Á‚½‚ç
		if (data->GetStateName() == state && data->GetWeaponType() == type)
		{
			path += data->GetAnimName();
			break;
		}
	}

	return path;
}

std::wstring CharacterBase::GetEffectPath(std::wstring effectName) const
{
	std::wstring path;

	//’T‚·
	for (auto& data : m_effectDatas)
	{
		//ðŒ‚É‡‚¤‚à‚Ì‚ª‚ ‚Á‚½‚ç
		if (data->GetName() == effectName)
		{
			path = data->GetPath();
			break;
		}
	}

	return path;
}

std::wstring CharacterBase::GetSEPath(std::wstring seName) const
{
	std::wstring path;

	//’T‚·
	for (auto& data : m_seDatas)
	{
		//ðŒ‚É‡‚¤‚à‚Ì‚ª‚ ‚Á‚½‚ç
		if (data->GetName() == seName)
		{
			path = data->GetPath();
			break;
		}
	}

	return path;
}

std::wstring CharacterBase::GetVoicePath(std::wstring voiceName) const
{
	std::wstring path;

	//’T‚·
	for (auto& data : m_voiceDatas)
	{
		//ðŒ‚É‡‚¤‚à‚Ì‚ª‚ ‚Á‚½‚ç
		if (data->GetName() == voiceName)
		{
			path = data->GetPath();
			break;
		}
	}

	return path;
}

void CharacterBase::ChangeArmor(CharaStatus::Armor armor)
{
	m_charaStatus->SetArmor(armor);
}

void CharacterBase::InitArmor()
{
	m_charaStatus->InitArmor();
}

Vector3 CharacterBase::GetCenterPos() const
{
	Vector3 start = m_rb->GetPos();
	Vector3 end = std::dynamic_pointer_cast<CapsuleCollider>(m_collisionData)->GetEndPos();
	return (start + end) * 0.5f;
}

void CharacterBase::Wait()
{
	if (!m_state)return;
	m_state->Wait();
}

void CharacterBase::Operate()
{
	if (!m_state)return;
	m_state->Operate();
}

void CharacterBase::InitAttackData(CSVDataLoader& csvLoader, std::wstring path)
{
	m_attackDatas.clear();
	auto datas = csvLoader.LoadCSV(path.c_str());
	//“o˜^
	for (auto& data : datas)
	{
		std::shared_ptr<AttackData> attackData = std::make_shared<AttackData>(data);
		m_attackDatas.emplace_back(attackData);
	}
}

void CharacterBase::InitAnimData(CSVDataLoader& csvLoader,std::wstring path)
{
	m_animDatas.clear();
	auto datas = csvLoader.LoadCSV(path.c_str());
	//“o˜^
	for (auto& data : datas)
	{
		std::shared_ptr<AnimData> animData = std::make_shared<AnimData>(data);
		m_animDatas.emplace_back(animData);
	}
}

void CharacterBase::InitEffectData(CSVDataLoader& csvLoader, std::wstring effectPath)
{
	m_effectDatas.clear();
	auto effectDatas = csvLoader.LoadCSV(effectPath.c_str());
	//“o˜^
	for (auto data : effectDatas)
	{
		auto effectData = std::make_shared<EffectData>(data);
		m_effectDatas.emplace_back(effectData);
		AssetManager::GetInstance().GetEffectHandle(effectData->GetPath());
	}
}

void CharacterBase::InitSE(CSVDataLoader& csvLoader, std::wstring sePath)
{
	auto& soundManager = SoundManager::GetInstance();

	auto seDatas = csvLoader.LoadCSV(sePath.c_str());
	//“o˜^
	for (auto data : seDatas)
	{
		auto seData = std::make_shared<SoundData>(data);
		m_seDatas.emplace_back(seData);
		soundManager.LoadSE(seData->GetPath());
	}
}

void CharacterBase::InitVoice(CSVDataLoader& csvLoader, std::wstring voicePath)
{
	auto& soundManager = SoundManager::GetInstance();

	auto voiceDatas = csvLoader.LoadCSV(voicePath.c_str());
	//“o˜^
	for (auto data : voiceDatas)
	{
		auto voiceData = std::make_shared<SoundData>(data);
		m_voiceDatas.emplace_back(voiceData);
		soundManager.LoadVoice(voiceData->GetPath());
	}
}
