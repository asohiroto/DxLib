#pragma once
#include "../HPUIBase.h"
#include "../../../General/Math/MyMath.h"
#include <memory>
class CharaStatus;
class PlayerHPUI :
    public HPUIBase
{
public:
	PlayerHPUI(std::shared_ptr<CharaStatus> charaStatus);
	~PlayerHPUI();
	//XV
	void Update() override;
	//•`‰æ
	void Draw()const override;
private:
	//•`‰æÀ•W
	Vector2 m_pos;
};

