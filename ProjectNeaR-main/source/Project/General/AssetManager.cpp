#include "AssetManager.h"
#include "LoadingManager.h"
#include <cassert>
#include <DxLib.h>
#include <EffekseerForDXLib.h>

namespace
{
    //モデル
    const std::wstring kModelPath = L"Data/Model/";
    const std::wstring kMV1 = L".mv1";

    //エフェクト
    const std::wstring kEffectPath = L"Data/Effect/";
    const std::wstring kEfk = L".efkefc";

    //画像
    const std::wstring kImagePath = L"Data/UI/";
    const std::wstring kPng = L".png";

    //フォントパス
    const std::string kCinzelPath = "Data/Font/Cinzel-Medium.ttf";
    const std::string kNotoPath = "Data/Font/NotoSansJP-Light.ttf";
    const std::string kRobotoPath = "Data/Font/Roboto-Light.ttf";
    //フォントの名前
    const std::wstring kCinzelName = L"Cinzel Medium";
    const std::wstring kNotoName = L"Noto Sans JP Light";
    const std::wstring kRobotoName = L"Roboto Light";

    //音
    const std::wstring kSoundPath = L"Data/Sound/";
    const std::wstring kMP3 = L".mp3";
}

void AssetManager::Init()
{
    //フォントの準備
    assert(AddFontResourceExA(kCinzelPath.c_str(), FR_PRIVATE, NULL) > 0);
    assert(AddFontResourceExA(kNotoPath.c_str(), FR_PRIVATE, NULL) > 0);
    assert(AddFontResourceExA(kRobotoPath.c_str(), FR_PRIVATE, NULL) > 0);

    m_isLoading = false;
}
void AssetManager::Update()
{
#if _DEBUG
   /* printf("AssetNum = %d\n",
        m_imageHandles.size() +
        m_fontHandles.size() +
        m_modelHandles.size() +
        m_soundHandles.size() +
        m_modelHandles.size());*/
#endif


    //ロード中じゃないなら早期リターン
    if (!LoadingManager::GetInstance().IsLoading())return;

    //全ての非同期ロードが終わったかをチェック
    m_isLoading = true;
    for (const auto& [key, value] : m_imageHandles) 
    {
        //ロードが完了していないなら
        auto result = CheckHandleASyncLoad(value);
        if (result > 0)return;
    }
    for (const auto& [key, value] : m_fontHandles) 
    {
        //ロードが完了していないなら
        auto result = CheckHandleASyncLoad(value);
        if (result > 0)return;
    }
    for (const auto& [key, value] : m_modelHandles) 
    {
        //ロードが完了していないなら
        auto result = CheckHandleASyncLoad(value);
        if (result > 0)return;
    }
    for (const auto& [key, value] : m_soundHandles) 
    {
        //ロードが完了していないなら
        auto result = CheckHandleASyncLoad(value);
        if (result > 0)return;
    }

    //ここまで来たら全てのロードが完了
    m_isLoading = false;
}

void AssetManager::End()
{
    //ウィンドウズに一時的に保持していたフォントデータを削除
    RemoveFontResourceExA("", FR_PRIVATE, NULL);
}

int AssetManager::GetModelHandle(std::wstring path)
{
    int handle = -1;
    //モデルがあったら
    if (m_modelHandles.find(path) != m_modelHandles.end())
    {
        handle = m_modelHandles.at(path);
    }
    else
    {
        //ハンドルをロードする
        std::wstring loadPath = kModelPath + path + kMV1;
        handle = MV1LoadModel(loadPath.c_str());

        //ハンドルチェック
        if (handle == -1)return handle;

        m_modelHandles[path] = handle;
    }
  
    //コピーを渡す
    return MV1DuplicateModel(handle);
}

void AssetManager::DeleteModelHandle()
{
    //ハンドルをすべて削除
    for (auto& [key, value] : m_modelHandles) 
    {
        if (value == -1)continue;
        MV1DeleteModel(value);
        value = -1;
    }
    m_modelHandles.clear();
}

int AssetManager::GetEffectHandle(std::wstring path)
{
    int handle = -1;
    //エフェクトがあったら
    if (m_effectHandles.find(path) != m_effectHandles.end())
    {
        handle = m_effectHandles.at(path);
    }
    else
    {
        //非同期ロードをしているか
		bool isASyncLoad = GetUseASyncLoadFlag();
		//非同期ロードを無効にする
		SetUseASyncLoadFlag(false);

        //ハンドルをロードする
        std::wstring loadPath = kEffectPath + path + kEfk;
        handle = LoadEffekseerEffect(loadPath.c_str());

        //ハンドルチェック
        if (handle == -1)return handle;

        m_effectHandles[path] = handle;

		//非同期ロードの設定を元に戻す
		SetUseASyncLoadFlag(isASyncLoad);
    }

    int playHandle = PlayEffekseer3DEffect(handle);
    if (playHandle == -1)return playHandle;

    //ハンドルを渡す
    return playHandle;
}

void AssetManager::DeleteEffectHandle()
{
    //ハンドルをすべて削除
    for (auto& [key, value] : m_effectHandles) {
        if (value == -1)continue;
        DeleteEffekseerEffect(value);
        value = -1;
    }
    m_effectHandles.clear();
}

int AssetManager::GetImageHandle(std::wstring path)
{
    int handle = -1;
    //画像があったら
    if (m_imageHandles.find(path) != m_imageHandles.end())
    {
        handle = m_imageHandles.at(path);
    }
    else
    {
        //ハンドルをロードする
        std::wstring loadPath = kImagePath + path + kPng;
        handle = LoadGraph(loadPath.c_str());

        //ハンドルチェック
        if (handle == -1)return handle;

        m_imageHandles[path] = handle;
    }

    return handle;
}

void AssetManager::DeleteImageHandle()
{
    //ハンドルをすべて削除
    for (auto& [key, value] : m_imageHandles) {
        if (value == -1)continue;
        DeleteGraph(value);
        value = -1;
    }
    m_imageHandles.clear();
}

int AssetManager::GetSoundHandle(std::wstring path)
{
    int handle = -1;
    //サウンドがあったら
    if (m_soundHandles.find(path) != m_soundHandles.end())
    {
        handle = m_soundHandles.at(path);
    }
    else
    {
        //ハンドルをロードする
        std::wstring loadPath = kSoundPath + path + kMP3;
        handle = LoadSoundMem(loadPath.c_str());

        //ハンドルチェック
        assert(handle != -1);
        if (handle == -1)return handle;

        m_soundHandles[path] = handle;
    }

    return DuplicateSoundMem(handle);
}

void AssetManager::DeleteSoundHandle()
{
    //ハンドルをすべて削除
    for (auto& [key, value] : m_soundHandles) {
        if (value == -1)continue;
        DeleteSoundMem(value);
        value = -1;
    }
    m_soundHandles.clear();
}

int AssetManager::GetFontHandle(Font font)
{
    int handle = -1;
    //フォントがあったら
    if (m_fontHandles.find(font) != m_fontHandles.end())
    {
        handle = m_fontHandles.at(font);
    }
    else
    {
        auto type = font.type;
        std::wstring name;
        switch (type)
        {
        case AssetManager::FontType::Cinzel:
            name = kCinzelName;
            break;
        case AssetManager::FontType::NotoSansJP:
            name = kNotoName;
            break;
        case AssetManager::FontType::Roboto:
            name = kRobotoName;
            break;
        default:
            name = kCinzelName;
            break;
        }

        //ハンドルをロードする
        handle = CreateFontToHandle(name.c_str(), static_cast<int>(font.size), 5, DX_FONTTYPE_ANTIALIASING);

        //ハンドルチェック
        if (handle == -1)return handle;

        m_fontHandles[font] = handle;
    }


    return handle;
}

void AssetManager::DeleteFontHandle()
{
    for (auto& [key, value] : m_fontHandles) {
        if (value == -1)continue;
        DeleteFontToHandle(value);
        value = -1;
    }
    m_fontHandles.clear();
}

void AssetManager::AllDelete()
{
    //ロード中はスキップ
    if (LoadingManager::GetInstance().IsLoading())return;

    DeleteImageHandle();
    DeleteEffectHandle();
    DeleteModelHandle();
    DeleteFontHandle();
    DeleteSoundHandle();
}
