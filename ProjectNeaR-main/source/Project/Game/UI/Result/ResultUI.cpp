#include "ResultUI.h"
#include "../../../General/AssetManager.h"
#include "../../../General/Game.h"
#include "../../../General/Math/MyMath.h"
#include "../../../General/Timer.h"
#include "../../../General/CSV/CSVDataLoader.h"
#include "../../../General/CSV/ResultRankData.h"
#include "../../../General/CSV/ClearSaveData.h"
#include "../../../General/CSV/HighScoreData.h"
#include "../../../General/SaveDataManager.h"
#include <DxLib.h>
#include <cmath>
#include <sstream>
#include <iomanip>
namespace
{
    //テキストカラー
    constexpr int kTextColor = 0xe1e1df;

    //Alpha速度
    constexpr int kAlphaSpeed = 5;

    //リザルトの文字
    constexpr int kResultTextPosX = Game::kScreenCenterX;
    constexpr int kResultTextPosY = 150;
    
    //リザルトの情報
    constexpr int kResultInfoPosX = Game::kScreenCenterX;
    constexpr int kResultInfoPosY = Game::kScreenCenterY;
    //タイム
    constexpr int kTimePosX = Game::kScreenCenterX + 90;
    constexpr int kTimePosY = Game::kScreenCenterY - 55;
    //桁数
    constexpr int kDigitsNum = 2;

    //ランク
    constexpr int kRankPosX = Game::kScreenCenterX + 150;
    constexpr int kRankPosY = Game::kScreenCenterY + 20;

    //PressAnyBotton
    constexpr int kPressAnyBottonPosX = Game::kScreenCenterX;
    constexpr int kPressAnyBottonPosY = Game::kScreenHeight - 150.0f;

    //30分を最大とする
    constexpr int kTimeMaxHalfAnHour = 30;

    //リザルトの情報を表示するフレーム
    constexpr int kDisplayResultInfoFrame = 45;

    //透明度の最高
    constexpr int kAlphaMax = 255;

    //リザルトランクのパス
    const std::wstring kResultRankPath = L"/ResultRankData";
    //パス
    const std::wstring kResultImagePath = L"Result/Result";
    const std::wstring kResultInfoImagePath = L"Result/ResultInfo";
    const std::wstring kPressAnyBottonImagePath = L"Result/PressAnyBotton";
    const std::wstring kBackImagePath = L"Back/BlackBack2";

    //終了フレーム
	constexpr int kFinishFrame = 150;
}

ResultUI::ResultUI(std::wstring stageName, std::shared_ptr<Timer> timer):
    m_resultHandle(-1),
    m_backHandle(-1),
    m_resultInfoHandle(-1),
    m_fontHandle(-1),
    m_countFrame(0),
    m_timer(timer),
    m_rank(L"E"),
    m_timeText(L"")
{
    auto& assetManager = AssetManager::GetInstance();
    m_resultHandle.handle = assetManager.GetImageHandle(kResultImagePath);
    m_resultInfoHandle.handle = assetManager.GetImageHandle(kResultInfoImagePath);
    m_pressAnyBottonHandle.handle = assetManager.GetImageHandle(kPressAnyBottonImagePath);
    m_backHandle = assetManager.GetImageHandle(kBackImagePath);
    m_fontHandle = assetManager.GetFontHandle(AssetManager::Font(AssetManager::FontType::Roboto, AssetManager::FontSize::Size36));

    //ランクを計算
    int min = m_timer->GetMinutes();
    int sec = m_timer->GetSeconds();
    //クリアタイム
    float clearTime = min * 60 + sec;
   
    auto& csvLoader = CSVDataLoader::GetInstance();
    std::wstring path = stageName + kResultRankPath;
    auto datas = csvLoader.LoadCSV(path.c_str());
    for (auto data : datas)
    {
        if (!data)continue;
        auto resultRank = std::make_shared<ResultRankData>(data);

        //ランク条件タイム
        float rankTime = resultRank->GetMin() * 60 + resultRank->GetSec();

        //タイマーが条件を満たすタイムなら
        if (clearTime <= rankTime)
        {
            m_rank = resultRank->GetRank();
            break;
        }
    }

    int mins = m_timer->GetMillisecond();
    //30分を超えたら
    if (min >= kTimeMaxHalfAnHour)
    {
        //30分を上限とする
        min = kTimeMaxHalfAnHour;
        sec = 0;
        mins = 0;
    }

    std::wstringstream ss;
    //0埋め
    ss << std::setfill(L'0')
        << std::setw(kDigitsNum) << min << L":"
        << std::setw(kDigitsNum) << sec << L":"
        << std::setw(kDigitsNum) << mins;
    m_timeText = ss.str();

    //ステージインデックス
    StageIndex index = GetStageIndexByName(stageName);

    //セーブ
    auto& saveDataManager = SaveDataManager::GetInstance();

    //クリア状況
    auto clearData = saveDataManager.GetClearData();

    //スコア
    auto scoreData = saveDataManager.GetHighScoreData(index);
    
    //初クリア
    bool isNewClear = false;

    //初クリアかチェック
    switch (index)
    {
    case StageIndex::Stage1:
        isNewClear = !clearData->IsClearStage1();
        break;
    case StageIndex::Stage2:
        isNewClear = !clearData->IsClearStage2();
        break;
    case StageIndex::Stage3:
        isNewClear = !clearData->IsClearStage3();
        break;
    default:
        break;
    }

    //スコア更新
    if (isNewClear)
    {
        scoreData->SetRankText(m_rank);
        scoreData->SetTimeText(m_timeText);
    }
    else
    {
        //今回のタイムが最短ではないなら
        if (m_timer->GetTime() >= scoreData->GetTime())return;
        scoreData->SetRankText(m_rank);
        scoreData->SetTimeText(m_timeText);
    }
}

ResultUI::~ResultUI()
{
}

void ResultUI::Update()
{
    ++m_countFrame;
    m_resultHandle.alpha += kAlphaSpeed;
    if (m_countFrame >= kDisplayResultInfoFrame)
    {
        m_resultInfoHandle.alpha += kAlphaSpeed;
    }
    if (m_resultInfoHandle.alpha >= kAlphaMax)
    {
        m_pressAnyBottonHandle.alpha = (std::cos(m_countFrame * kAlphaSpeed * MyMath::DEG_2_RAD) * kAlphaMax);
    }
}

void ResultUI::Draw() const
{
    if (!m_isDraw)return;
    //背景
    DrawRotaGraph(Game::kScreenCenterX, Game::kScreenCenterY, 1.0, 0.0, m_backHandle, true);

    //リザルトの文字
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_resultHandle.alpha);
    DrawRotaGraph(kResultTextPosX, kResultTextPosY, 1.0, 0.0, m_resultHandle.handle, true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    //リザルトの情報
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_resultInfoHandle.alpha);
    DrawRotaGraph(kResultInfoPosX, kResultInfoPosY, 1.0, 0.0, m_resultInfoHandle.handle, true);

    //タイム
    DrawStringToHandle(kTimePosX, kTimePosY, m_timeText.c_str(), kTextColor, m_fontHandle);

    //ランク
    DrawStringToHandle(kRankPosX, kRankPosY, m_rank.c_str(), kTextColor, m_fontHandle);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    //PressAnyBottonHandle
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, m_pressAnyBottonHandle.alpha);
    DrawRotaGraph(kPressAnyBottonPosX, kPressAnyBottonPosY, 1.0, 0.0, m_pressAnyBottonHandle.handle, true);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

bool ResultUI::IsFinish()const
{
    return m_countFrame >= kFinishFrame;
}
