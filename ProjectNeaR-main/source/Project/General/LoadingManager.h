#pragma once
#include <vector>
#include <string>
/// <summary>
/// シングルトン
/// </summary>
class LoadingManager final
{
private:
	//シングルトンの準備
	LoadingManager() = default;
	~LoadingManager() = default;
	//コピー禁止
	LoadingManager(const LoadingManager&) = delete;
	LoadingManager& operator = (const LoadingManager&) = delete;
	//ムーブ禁止
	LoadingManager(LoadingManager&&) = delete;
	LoadingManager& operator = (LoadingManager&&) = delete;
public:
	//インスタンスを取得
	static LoadingManager& GetInstance()
	{
		static LoadingManager instance;
		return instance;
	}
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw();
	//終了処理
	void End();
	//非同期ロード開始
	void StartLoading();
	//非同期ロード終了
	void EndLoading();
	//非同期ロード中かどうか
	bool IsLoading()const { return m_isLoading; };

private:

	//ロード中かどうか
	bool m_isLoading = false;

	//非同期ロード数
	int m_totalLoadNum = 0;

	//ロード時間カウント
	int m_loadCountFrame = 0;
	int m_afterLoadFrame = 0;

	//割合
	int m_loadingRate = 0;

	//ギアの回転
	float m_gearRotaAngle = 0.0f;

	//Loadingの.を一定間隔で出すための変数
	int m_loadingPointCountFrame = 0;

	//フォントハンドル
	int m_fontHandle = -1;
	//画像ハンドル
	int m_backHandle = -1;
	int m_loadingGearHandle = -1;
	int m_loadingTextHandle = -1;
	int m_loadingTextPointHandle = -1;

	//テキスト
	std::vector<std::wstring> m_loadingTexts;
};

