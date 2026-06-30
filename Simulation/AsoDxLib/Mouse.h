#pragma once
#include"DxLib.h"

namespace Mouse
{
	inline int nowMouse = 0;
	inline int lastMouse = 0;

	// 更新処理
	static inline void Update()
	{
		lastMouse = nowMouse;		// 前の入力を現在の入力にする
		nowMouse = GetMouseInput(); // 最新の入力を取得 
	}

	// 押されているか
	static inline bool IsPress(int key)
	{
		bool isNow = (nowMouse & key);

		return isNow;
	}

	// クリックされた瞬間を取得
	static inline bool IsTrigger(int key)
	{
		bool isNow = (nowMouse & key);   // 今押されているか
		bool isLast = (lastMouse & key); // １フレーム前に押されていたか

		return (isNow && !isLast);
	}

	// 離した瞬間を取得
	static inline bool IsRelease(int key)
	{
		bool isNow = (nowMouse & key);
		bool isLast = (lastMouse & key);

		return (!isNow && isLast);
	}
}