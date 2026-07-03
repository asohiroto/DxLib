#pragma once
#include "../UIBase.h"
class InputUI :
    public UIBase
{
public:
	InputUI();
	~InputUI();
	//XV
	void Update() override;
	//•`‰æ
	void Draw()const override;
private:
	int m_handle;
};

