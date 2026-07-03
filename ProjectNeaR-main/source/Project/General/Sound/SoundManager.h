#pragma once
#include <map>
#include <string>
#include <list>
#include <memory>
class SoundBase;
class BGM;
class SE;
class Voice;
/// <summary>
/// 音(BGM,SE)を管理するシングルトンクラス
/// </summary>
class SoundManager final
{
private:
	//シングルトンの準備
	SoundManager() = default;
	~SoundManager() = default;
	//コピー禁止
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator = (const SoundManager&) = delete;
	//ムーブ禁止
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator = (SoundManager&&) = delete;
public:
	//インスタンスを取得
	static SoundManager& GetInstance()
	{
		static SoundManager instance;
		return instance;
	}
	//登録
	void Entry(std::shared_ptr<SoundBase> sound);
	//登録解除
	void Exit(std::shared_ptr<SoundBase> sound);
	//初期化
	void Init();
	//更新
	void Update();
	//終了
	void End();
	//リセット
	void Reset();
	//BGM再生
	void PlayBGM(std::wstring path);
	void PlayBGM();
	//BGM停止
	void StopBGM();
	//1度だけ再生する 
	std::weak_ptr<SE> PlayOnceSE(std::wstring path);
	//ループ再生する
	std::weak_ptr<SE> PlayLoopSE(std::wstring path);
	//1度だけ再生する 
	std::weak_ptr<Voice> PlayVoice(std::wstring path);
	//全ての再生を開始
	void AllPlay();
	//全ての再生を止める
	void AllStop();
	//BGMのロード
	int LoadBGM(std::wstring path);
	//SEのロード
	int LoadSE(std::wstring path);
	//Voiceのロード
	int LoadVoice(std::wstring path);
	//音量定数
	enum class SoundVolume : int
	{
		V0 = 0,
		V1 = 20,
		V2 = 40,
		V3 = 60,
		V4 = 80,
		V5 = 100,
		V6 = 120,
		V7 = 140,
		V8 = 160,
		V9 = 180,
		V10 = 200,
	};

	//セッター
	void SetSEVolume(SoundVolume volume);
	void SetBGMVolume(SoundVolume volume);
	void SetVoiceVolume(SoundVolume volume);
	
	//レベル加算・減算
	void LevelUpSEVolume();
	void LevelDownSEVolume();
	void LevelUpBGMVolume();
	void LevelDownBGMVolume();
	void LevelUpVoiceVolume();
	void LevelDownVoiceVolume();


	//音量
	SoundVolume GetSEVolume()	 const { return m_seVolume; };
	SoundVolume GetBGMVolume()	 const { return m_bgmVolume; };
	SoundVolume GetVoiceVolume() const { return m_voiceVolume; };
	//音量レベル
	int GetLevelToVolume(SoundVolume volume)const;
	SoundVolume GetVolumeToLevel(int level)const;
	
	
private:
	//SEの音量
	SoundVolume m_seVolume = SoundVolume::V5;
	//BGMの音量
	SoundVolume m_bgmVolume = SoundVolume::V5;
	//ボイスの音量
	SoundVolume m_voiceVolume = SoundVolume::V5;
	//サウンド
	std::list<std::shared_ptr<SoundBase>> m_sounds;
	//BGMは常に一つなのでここで扱う
	std::shared_ptr<BGM> m_bgm;
private:
	//削除候補のものを削除
	void CheckDeleteSound();
};
