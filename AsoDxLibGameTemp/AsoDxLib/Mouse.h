#pragma once
#include"DxLib.h"

namespace
{
	int nowMouse = 0;
	int lastMouse = 0;
}

namespace Mouse
{

	// 更新処理
	void Update()
	{
		lastMouse = nowMouse;		// 前の入力を現在の入力にする
		nowMouse = GetMouseInput(); // 最新の入力を取得 
	}

	// 押されているか
	bool IsPress(int key)
	{
		bool isNow = (nowMouse & key);

		return isNow;
	}

	// クリックされた瞬間を取得
	bool IsTrigger(int key)
	{
		bool isNow = (nowMouse & key);   // 今押されているか
		bool isLast = (lastMouse & key); // １フレーム前に押されていたか

		return (isNow && !isLast);
	}

	// 離した瞬間を取得
	bool IsRelease(int key)
	{
		bool isNow = (nowMouse & key);
		bool isLast = (lastMouse & key);

		return (!isNow && isLast);
	}
}