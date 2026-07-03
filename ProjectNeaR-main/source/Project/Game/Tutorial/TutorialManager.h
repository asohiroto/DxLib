#pragma once
#include <memory>
#include <list>
#include <string>
class Player;
class TalkUI;
class TutorialClearData;
class ActorManager;
class TutorialManualUI;
class TutorialManager
{
public:
	TutorialManager(std::weak_ptr<Player> pPlayer,std::shared_ptr<ActorManager> pActorManager,std::wstring stageName);
	~TutorialManager();
	//更新
	void Update();

	//チュートリアルをクリアしたら
	bool IsClear()const;
private:
	//プレイヤー
	std::weak_ptr<Player> m_pPlayer;
	//テキストUI
	std::weak_ptr<TalkUI> m_pTextUI;
	//操作説明UI
	std::weak_ptr<TutorialManualUI> m_pTutorialManualUI;
	//アクターマネージャー
	std::shared_ptr<ActorManager> m_pActorManager;
	//クリア情報
	std::shared_ptr<TutorialClearData> m_clearData;
	//クリアしたか
	bool m_isClear;

	//クリアしたと表示するのを少し遅らせる
	int m_clearDelayFrame;
private:
	//説明が終わって数フレーム動けないようにする
	int m_movingDelayFrame;
};

