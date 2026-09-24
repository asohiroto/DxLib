#pragma once
#include"DxLib.h"

namespace Pad
{
	inline int lastPad = 0;
	inline int nowPad = 0;

	// 更新処理
	static inline void Update()
	{
		// 前のフレームに取得したパッド情報を一つ古い状態にする
		lastPad = nowPad;
		// 現在のパッド情報を取得する
		nowPad = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	}

	// 押されているか
	static inline bool IsPress(int key)
	{
		// 現在のフレームで押されている
		return(nowPad & key);
	}

	// 押された瞬間を取得
	static inline bool IsTrigger(int key)
	{
		bool isNow = (nowPad & key);	// 現在のフレーム
		bool isLast = (lastPad & key);  // 前のフレーム

		// 現在のフレームに押されていて、前のフレームに押されていない
		return (isNow && !isLast);
	}

	// 離した瞬間を取得
	static inline bool IsRelease(int key)
	{
		bool isNow = (nowPad & key);	// 現在のフレーム
		bool isLast = (lastPad & key);	// 前のフレーム

		// 現在のフレームに押されていなくて、前のフレームは押されていた
		return (!isNow && isLast);
	}
}