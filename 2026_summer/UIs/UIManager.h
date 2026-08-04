#pragma once
#include <memory>

class CrossHair;

class UIManager
{
public:
	UIManager();
	virtual ~UIManager();
	void Init();
	void End();
	void Update();
	void Draw();

private:
	// クロスヘア管理のポインタ
	std::shared_ptr<CrossHair> p_CrossHair;
};