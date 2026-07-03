#pragma once
#include <map>
#include <string>
/// <summary>
/// リソース管理を行うクラス
/// </summary>
class AssetManager final
{
private:
	//シングルトンの準備
	AssetManager() = default;
	~AssetManager() = default;
	//コピー禁止
	AssetManager(const AssetManager&) = delete;
	AssetManager& operator = (const AssetManager&) = delete;
	//ムーブ禁止
	AssetManager(AssetManager&&) = delete;
	AssetManager& operator = (AssetManager&&) = delete;
public:
	//インスタンスを取得
	static AssetManager& GetInstance()
	{
		static AssetManager instance;
		return instance;
	}

	//初期化
	void Init();
	//更新
	void Update();
	//終了
	void End();

	//モデルの取得
	int GetModelHandle(std::wstring path);
	//モデルのリソースの削除
	void DeleteModelHandle();

	//エフェクトの取得
	int GetEffectHandle(std::wstring path);
	//エフェクトのリソースの削除
	void DeleteEffectHandle();

	//画像ハンドル
	int GetImageHandle(std::wstring path);
	//画像ハンドル削除
	void DeleteImageHandle();

	//音ハンドル
	int GetSoundHandle(std::wstring path);
	//音ハンドル削除
	void DeleteSoundHandle();

	//フォントタイプ
	enum class FontType : int
	{
		Cinzel = 1,
		NotoSansJP = 2,
		Roboto = 3
	};
	enum class FontSize : int
	{
		Size8 = 8,
		Size12 = 12,
		Size16 = 16,
		Size20 = 20,
		Size24 = 24,
		Size28 = 28,
		Size32 = 32,
		Size36 = 36,
		Size40 = 40,
		Size44 = 44,
		Size48 = 48,
		Size52 = 52,
		Size56 = 56,
		Size64 = 64,
	};
	struct Font
	{
		Font()
		{
			type = FontType::Cinzel;
			size = FontSize::Size8;
		}
		/// <summary>
		/// フォント作成
		/// </summary>
		/// <param name="t">タイプ</param>
		/// <param name="s">サイズ</param>
		Font(FontType t, FontSize s)
		{
			type = t;
			size = s;
		}
		FontType type;
		FontSize size;

		bool operator < (const Font& other) const
		{
			if (type != other.type)
			{
				return type < other.type;
			}
			return size < other.size;
		}
	};

	//フォント
	int GetFontHandle(Font font);
	//フォント削除
	void DeleteFontHandle();

	//全て削除
	void AllDelete();

	//ロード中か
	bool IsLoading()const { return m_isLoading; };

private:
	//モデルリソース
	std::map<std::wstring,int>m_modelHandles;
	//エフェクトリソース
	std::map<std::wstring, int>m_effectHandles;
	//画像ハンドル
	std::map<std::wstring, int>m_imageHandles;
	//フォントハンドル
	std::map<Font, int>m_fontHandles;
	//音リソース
	std::map<std::wstring, int>m_soundHandles;

	//ロードしているか
	bool m_isLoading = false;
};

