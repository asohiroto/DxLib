#include "CSVDataSaver.h"
#include "../StringUtil.h"
#include "../Sound/SoundManager.h"
#include "../SaveDataManager.h"
#include "../../Main/Application.h"
#include "ClearSaveData.h"
#include "HighScoreData.h"
#include "TimeSaveData.h"
#include <iostream>
#include <fstream>
#include <list>
#include <cassert>

namespace
{
    //パス
    const std::string kOptionPath = "Data/CSV/Save/OptionSaveData.csv";
    const std::string kClearSaveDataPath = "Data/CSV/Save/ClearSaveData.csv";
    const std::string kHighScoreSaveDataPath = "Data/CSV/Save/HighScoreSaveData.csv";
    const std::string kTimeSaveDataPath = "Data/CSV/Save/TimeSaveData.csv";

    //オプションヘッダー
    const std::string kOptionSaveHeader = "BGM,SE,Voice,ScreenMode\n";

    //クリアヘッダー
    const std::string kClearSaveDataHeader = "Tutorial1,Tutorial2,Tutorial3,Stage1,Stage2,Stage3\n";

    //ハイスコアヘッダー
    const std::string kHighScoreSaveDataHeader = "StageIndex,Time,Rank\n";

    //日付と総プレイ時間データのヘッダー
    const std::string kTimeSaveDataPathHeader = "TotalTime,LastPlayDate\n";
}

void CSVDataSaver::SaveOption()
{
    //ファイルを開く(ない場合は作成される)
    std::ofstream file(kOptionPath);
    //ファイルが読み込めなかったら
    if (!file)
    {
        assert("ファイルの読み込みに失敗");
    }

    //ヘッダーを書き込む
    file << kOptionSaveHeader;

    //サウンド
    auto& soundManager = SoundManager::GetInstance();

    //各サウンドレベル
    int bgmLv = soundManager.GetLevelToVolume(soundManager.GetBGMVolume());
    int seLv = soundManager.GetLevelToVolume(soundManager.GetSEVolume());
    int voiceLv = soundManager.GetLevelToVolume(soundManager.GetVoiceVolume());

    //アプリケーション
    auto& app = Application::GetInstance();

    //スクリーンモード
    int isWindow = static_cast<int>(app.IsWindowMode());

    //データ書き込み
    file << bgmLv << ","
        << seLv << ","
        << voiceLv << ","
        << isWindow <<
        std::endl;
  
    //閉じる
    file.close();
}

void CSVDataSaver::SaveClearData()
{
    //ファイルを開く(ない場合は作成される)
    std::ofstream file(kClearSaveDataPath);
    //ファイルが読み込めなかったら
    if (!file)
    {
        assert("ファイルの読み込みに失敗");
    }

    //ヘッダーを書き込む
    file << kClearSaveDataHeader;

    //セーブデータ
    auto clearData = SaveDataManager::GetInstance().GetClearData();

    //データ書き込み
    file << static_cast<int>(clearData->IsClearTutorial1()) << ","
        << static_cast<int>(clearData->IsClearTutorial2()) << ","
        << static_cast<int>(clearData->IsClearTutorial3()) << ","
        << static_cast<int>(clearData->IsClearStage1()) << ","
        << static_cast<int>(clearData->IsClearStage2()) << ","
        << static_cast<int>(clearData->IsClearStage3()) <<
        std::endl;

    //閉じる
    file.close();
}

void CSVDataSaver::SaveHighScoreData()
{
    //ファイルを開く(ない場合は作成される)
    std::ofstream file(kHighScoreSaveDataPath);
    //ファイルが読み込めなかったら
    if (!file)
    {
        assert("ファイルの読み込みに失敗");
    }

    //ヘッダーを書き込む
    file << kHighScoreSaveDataHeader;

    //ハイスコアデータ
    auto& saveDataManager = SaveDataManager::GetInstance();
    auto highScoreStage1Data = saveDataManager.GetHighScoreData(StageIndex::Stage1);
    auto highScoreStage2Data = saveDataManager.GetHighScoreData(StageIndex::Stage2);
    auto highScoreStage3Data = saveDataManager.GetHighScoreData(StageIndex::Stage3);

    //データ書き込み(ステージ1)
    file << 1 << ","
        << StringUtil::WstringToString(highScoreStage1Data->GetTimeText()) << ","
        << StringUtil::WstringToString(highScoreStage1Data->GetRankText()) <<
        std::endl;
    //データ書き込み(ステージ2)
    file << 2 << ","
        << StringUtil::WstringToString(highScoreStage2Data->GetTimeText()) << ","
        << StringUtil::WstringToString(highScoreStage2Data->GetRankText()) <<
        std::endl;
    //データ書き込み(ステージ3)
    file << 3 << ","
        << StringUtil::WstringToString(highScoreStage3Data->GetTimeText()) << ","
        << StringUtil::WstringToString(highScoreStage3Data->GetRankText()) <<
        std::endl;

    //閉じる 
    file.close();
}

void CSVDataSaver::SaveTimeData()
{
    //ファイルを開く(ない場合は作成される)
    std::ofstream file(kTimeSaveDataPath);
    //ファイルが読み込めなかったら
    if (!file)
    {
        assert("ファイルの読み込みに失敗");
    }

    //ヘッダーを書き込む
    file << kTimeSaveDataPathHeader;

    //セーブデータ
    auto timeSaveData = SaveDataManager::GetInstance().GetTimeSaveData();

    //データ書き込み
    file << timeSaveData->GetTotalPlayTime() << ","
        << StringUtil::WstringToString(timeSaveData->GetLastPlayDate()) <<
        std::endl;

    //閉じる
    file.close();
}
