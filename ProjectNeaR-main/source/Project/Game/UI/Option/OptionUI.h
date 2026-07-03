#pragma once
#include "../UIBase.h"
#include "../../../Scene/OptionScene.h"
#include <vector>
class OptionUI :
    public UIBase
{
public:
	OptionUI();
	~OptionUI();
	//更新
	void Update() override;
	//描画
	void Draw() const override;

	//選択中メニューのインデックスを設定
	void SetSelectMenuIndex(OptionScene::OptionMenu index);

	//ウィンドウモード切り替え
	void SetIsWindowMode(bool isWindow) { m_isScreenMode = isWindow; };
	void SetIsFullScreen(bool isFullScreen) { m_isFullScreen = isFullScreen; };
private:
	//画像ハンドル
	int m_bgmHandle;
	int m_seHandle;
	int m_voiceHandle;
	int m_volumeHandle;
	int m_screenModeHandle;
	int m_windowHandle;
	int m_fullScreenHandle;
	int m_gameBackHandle;
	int m_cursorHandle;
	int m_backHandle;
	//フォントハンドル
	int m_fontHandle;

	//選択中メニュー
	OptionScene::OptionMenu m_selectMenuIndex;

	//座標
	std::vector<Vector2> m_menuPos;
	Vector2 m_cursorPos;

	//各ボリューム
	int m_bgmVolume;
	int m_seVolume;
	int m_voiceVolume;

	//ウィンドウモード
	bool m_isScreenMode;
	bool m_isFullScreen;


private:	
	//音量描画
	void DrawVolume(int volume,int index) const;
};

