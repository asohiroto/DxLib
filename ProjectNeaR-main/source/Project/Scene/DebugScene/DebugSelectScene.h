#pragma once
#include "../SceneBase.h"
#include "../../General/StageIndex.h"
class DebugSelectScene :
    public SceneBase
{
public:
	DebugSelectScene(SceneController& controller);
	~DebugSelectScene();
	/// <summary>
	/// 派生クラスで実装を実装
	/// virtual はこれを継承するかもしれないから
	/// overrideは明示的にオーバーライドをエディタに示すため
	/// </summary>
	virtual void Init()override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void End() override;
	virtual void Restart() override {};
	//デバッグ用
	virtual void DebugDraw() const override;
private:
	enum class Menu : int
	{
		Title		= 0,
		Select		= 1,
		Stage1		= 2,
		Stage2		= 3,
		Stage3		= 4,
		Result		= 5,
		Tutorial1	= 6,
		Tutorial2	= 7,
		Tutorial3	= 8,
		EndingScene = 9,
	};
	Menu m_currentMenu = Menu::Title;
};

