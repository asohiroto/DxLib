#pragma once
#include <memory>
class UIBase abstract:
	public std::enable_shared_from_this<UIBase>
{
public:
	UIBase();
	~UIBase();
	//‰Šú‰»
	virtual void Init();
	//I—¹
	virtual void End();
	//XV
	virtual void Update() abstract;
	//•`‰æ
	virtual void Draw()const abstract;
	//•`‰æ‡
	bool IsFrontDraw()const { return m_isFrontDraw; };
	//íœ
	bool IsDelete()const { return m_isDelete; };
	void Delete() { m_isDelete = true; };
	//•`‰æ‚µ‚Ä‚¢‚é‚©
	bool IsDraw()const { return m_isDraw; };
	// •`‰æ‚ğ—LŒø‚É‚·‚é
	void EnableDraw() { m_isDraw = true; };
	// •`‰æ‚ğ–³Œø‚É‚·‚é
	void DisableDraw() { m_isDraw = false; };
protected:
	//•`‰æ‡
	bool m_isFrontDraw;
	//íœ
	bool m_isDelete;
	//•`‰æ‚·‚é‚©
	bool m_isDraw;
};

