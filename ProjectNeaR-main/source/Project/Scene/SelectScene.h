#pragma once
#include "SceneBase.h"
#include <memory>
class Input;
class Fader;
class SelectUI;
class SelectCamera;
class ActorManager;
class SelectScene :
    public SceneBase
{
public:
	SelectScene(SceneController& controller);
	~SelectScene();
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
	enum class MainMenu : int
	{
		Tutorial = 0,
		Stage = 1,
		Option = 2,
		Title = 3,
		Max = 4,
	};
	enum class TutorialMenu : int
	{
		Tutorial1 = 0,
		Tutorial2 = 1,
		Tutorial3 = 2,
		Max = 3,
	};
	enum class StageMenu : int
	{
		Stage1 = 0,
		Stage2 = 1,
		Stage3 = 2,
		Max = 3,
	};

	//メインメニューの現在地
	MainMenu m_currentMainMenu = MainMenu::Tutorial;
	//チュートリアルメニューの現在地
	TutorialMenu m_currentTutorialMenu = TutorialMenu::Tutorial1;
	//ステージメニューの現在地
	StageMenu m_currentStageMenu = StageMenu::Stage1;
	//クリア状況からどこまでステージが解放されているか
	StageMenu m_canSelectStageMenu = StageMenu::Stage1;

	//アクターマネージャー
	std::shared_ptr<ActorManager> m_actorManager;

	//UI
	std::weak_ptr<SelectUI> m_selectStageUI;

	//カメラ
	std::weak_ptr<SelectCamera> m_selectCamera;

	//状態に合わせた更新
	using UpdateFunc = void(SelectScene::*)(Input& input, Fader& fader,std::shared_ptr<SelectCamera> startCamera);
	UpdateFunc m_update;
	
private:
	//メインメニュー更新
	void UpdateMainMenu(Input& input, Fader& fader, std::shared_ptr<SelectCamera> startCamera);
	//チュートリアルメニュー更新
	void UpdateTutorialMenu(Input& input, Fader& fader, std::shared_ptr<SelectCamera> startCamera);
	//ステージメニュー更新
	void UpdateStageMenu(Input& input, Fader& fader, std::shared_ptr<SelectCamera> startCamera);
};

