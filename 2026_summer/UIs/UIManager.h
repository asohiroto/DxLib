#pragma once
#include <memory>

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

};