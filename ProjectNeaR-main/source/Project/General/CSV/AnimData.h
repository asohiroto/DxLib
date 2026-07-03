#pragma once
#include "CSVData.h"
#include "../Math/MyMath.h"
#include "../Collision/Collidable.h"
#include <string>
class AnimData :
	public CSVData
{
public:
	AnimData();
	AnimData(std::shared_ptr<CSVData> data);
	~AnimData();
	//武器
	enum class WeaponType
	{
		None,
		LightSword,
		BigSword,
	};
	//状態
	std::wstring GetStateName()const { return m_stateName; };
	//武器
	WeaponType GetWeaponType()const { return m_weaponType; };
	//アニメーション
	std::string GetAnimName()const { return m_animName; };
private:
	//変換
	void Conversion() override;

	//状態
	std::wstring m_stateName;
	//武器
	WeaponType m_weaponType;
	//アニメーション
	std::string m_animName;
};

