#pragma once

struct BgmHandles
{
	// タイトル画面のBGMハンドル
	int startBgmH;
	// 操作説明画面のBGMハンドル
	int explainBgmH;
	// 本編(戦闘)のBGMハンドル
	int mainBgmH;
	// リザルト画面のBGMハンドル
	int resultBgmH;
};

struct SeHandles
{
	// マジックショットのSEハンドル
	int shotH;
	// マジックミサイルのSEハンドル
	int missileH;
	// マジックフューリーのSEハンドル
	int furyH;
	// マジックビームのSEハンドル
	int beamH;
	// 被弾時のSEハンドル
	int hitH;
	// ジャスト回避成功時のSEハンドル
	int dodgeH;
	// 撃破時のSEハンドル
	int killH;
	// 魔法陣(チャージ中)のSEハンドル
	int circleH;
	// 必殺技チャージ１段階目のSEハンドル
	int firstChargeH;
	// 必殺技チャージ２段階目のSEハンドル
	int secondChargeH;
};