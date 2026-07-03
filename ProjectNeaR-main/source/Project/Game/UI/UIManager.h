#pragma once
#include <memory>
#include <list>
class UIBase;
class UIManager final
{
private:
	//シングルトンの準備
	UIManager() = default;
	~UIManager() = default;
	//コピー禁止
	UIManager(const UIManager&) = delete;
	UIManager& operator = (const UIManager&) = delete;
	//ムーブ禁止
	UIManager(UIManager&&) = delete;
	UIManager& operator = (UIManager&&) = delete;
public:
	//インスタンスを取得
	static UIManager& GetInstance()
	{
		static UIManager instance;
		return instance;
	}
	//登録
	void Entry(std::shared_ptr<UIBase> ui);
	//解除
	void Exit(std::shared_ptr<UIBase> ui);

	//更新
	void Update();
	//前描画
	void FrontDraw()const;
	//後描画
	void BackDraw()const;
	//削除
	void AllDeleteUI();

	//描画するか
	void SetIsDraw(bool isDraw) { m_isDraw = isDraw; };
private:
	std::list<std::shared_ptr<UIBase>> m_uis;
private:
	//削除チェック
	void CheckDelete();

	//描画するか
	bool m_isDraw = true;

	
};

