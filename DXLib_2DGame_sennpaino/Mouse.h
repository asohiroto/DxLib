#pragma once

namespace Mouse
{
	// 更新処理
	void Update();

	// 押されているか
	bool IsPress();

	// 右クリックされた瞬間を取得
	bool IsTriggerRight();

	// 左クリックされた瞬間を取得
	bool IsTriggerLeft();

	// 離した瞬間を取得
	bool IsRelease();
}