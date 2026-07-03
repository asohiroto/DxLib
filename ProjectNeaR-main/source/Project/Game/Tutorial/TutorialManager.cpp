#include "TutorialManager.h"
#include "../Actor/Character/Player/Player.h"
#include "../Actor/Character/Enemy/EnemyManager.h"
#include "../Actor/ActorManager.h"
#include "../../General/CSV/CSVDataLoader.h"
#include "../../General/CSV/TextData.h"
#include "../UI/TalkUI.h"
#include "../UI/Tutorial/TutorialManualUI.h"
#include "../UI/UIManager.h"
#include "../Camera/CameraController.h"
#include "../../General/Input.h"
#include "../../General/CSV/TutorialClearData.h"
#include <map>

namespace
{
	//パス
	const std::wstring kTextDataPath = L"Tutorial/TextData";
	const std::wstring kClearDataPath = L"Tutorial/TutorialClearData";
	//ジャスト回避数
	constexpr int kJustAvoidNum = 3;
	//クリア表示を遅らせる
	constexpr int kClearDelayFrame = 23;
	//しばらく待機
	constexpr int kMovingDelayFrame = 30;
}

TutorialManager::TutorialManager(std::weak_ptr<Player> pPlayer, std::shared_ptr<ActorManager> pActorManager,std::wstring stageName):
	m_pPlayer(pPlayer),
	m_pActorManager(pActorManager),
	m_isClear(false),
	m_clearDelayFrame(kClearDelayFrame),
	m_movingDelayFrame(kMovingDelayFrame)
{
	auto& csvLoader = CSVDataLoader::GetInstance();

	//チュートリアルクリア情報
	auto clearDatas = csvLoader.LoadCSV(kClearDataPath.c_str());
	for (auto& clearData : clearDatas)
	{
		auto tempData = std::make_shared<TutorialClearData>(clearData);
		if (tempData->GetTutorialName() == stageName)
		{
			m_clearData = tempData;
			break;
		}
	}

	//テキストデータ
	auto datas = csvLoader.LoadCSV(kTextDataPath.c_str());
	std::list<std::shared_ptr<TextData>> allTextDatas;
	
	//すべてのテキストを読み込む
	for (auto& data : datas)
	{
		auto textData = std::make_shared<TextData>(data);
		allTextDatas.push_back(textData);
	}

	//UIマネージャー
	auto& uiManager = UIManager::GetInstance();

	//UI作成の準備
	std::list<std::shared_ptr<TextData>> uiTextDatas;

	//グループ
	for (auto& textData : allTextDatas)
	{
		//メニューとあるテキストを取得
		if (textData->GetGroup() == stageName)
		{
			uiTextDatas.push_back(textData);
		}
	}

	//UI作成
	auto talkUI = std::make_shared<TalkUI>(uiTextDatas);
	talkUI->Init();
	m_pTextUI = talkUI;

	//説明UI
	auto tutorialManualUI = std::make_shared<TutorialManualUI>(GetStageIndexByName(stageName));
	tutorialManualUI->Init();
	tutorialManualUI->DisableDraw();//非表示
	m_pTutorialManualUI = tutorialManualUI;
}

TutorialManager::~TutorialManager()
{
}
void TutorialManager::Update()
{
	if (m_pPlayer.expired())return;
	if (!m_clearData)return;
	auto plyer = m_pPlayer.lock();

	//会話UIが消えたら
	if (m_pTextUI.expired())
	{
		//説明UIの描画
		if (!m_pTutorialManualUI.expired())
		{
			m_pTutorialManualUI.lock()->EnableDraw();
		}
		
		if (m_movingDelayFrame <= 0)
		{
			//行動可能
			plyer->Operate();
		}
		else
		{
			//カウント
			--m_movingDelayFrame;
		}

		//クリア条件がゴールなら
		switch (m_clearData->GetClearRequirement())
		{
		case TutorialClearData::ClearRequirement::AllDeadEnemy:
			m_isClear = m_pActorManager->GetEnemyManager()->IsAllDeadEnemys();
			break;
		case TutorialClearData::ClearRequirement::Goal:
			m_isClear = plyer->IsGoal();
			break;
		case TutorialClearData::ClearRequirement::JustAvoid:
			m_isClear = plyer->GetTotalJustAvoidNum() >= kJustAvoidNum;
			break;
		default:
			break;
		}
	}
	else
	{
		//待機
		plyer->Wait();
	}

	//クリアしたら
	if (m_isClear)
	{
		//カウントを減らす
		--m_clearDelayFrame;
	}
}

bool TutorialManager::IsClear() const
{
	return m_isClear && (m_clearDelayFrame <= 0);
}
