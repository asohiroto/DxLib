#include "DxLib.h"
#include "main.h" // ヘッダーファイルのインクルード
#include <stdlib.h>

// 定数の定義
const int WIDTH = 1200, HEIGHT = 720; // ウィンドウの幅と高さのピクセル数
const int FPS = 60; // フレームレート
const int IMG_ENEMY_MAX = 5; // 敵の画像の枚数（種類）
const int BULLET_MAX = 100;
const int ENEMY_MAX = 100; // 敵機の数の最大値
const int STAGE_DISTANCE = FPS * 60; // ステージの長さ
const int PLAYER_SHIELD_MAX = 8; // 自機のシールド最大値
const int EFFECT_MAX = 100; // エフェクトの最大数
const int ITEM_TYPE = 3; // アイテムの種類
const int WEAPON_LV_MAX = 10; // 武器レベルの最大値
const int PLAYER_SPEED_MAX = 20; // 自機の速さの最大値
enum { ENE_BULLET, ENE_ZAKO1, ENE_ZAKO2, ENE_ZAKO3, ENE_BOSS }; // 敵機の種類
enum { EFE_EXPLODE, EFE_RECOVER }; // エフェクトの最大数
enum { TITLE, PLAY, OVER, CLEAR }; // シーンを分けるための列挙定数

// グローバル変数
// ここでゲームに用いる変数や配列を定義する
int imgGalaxy, imgFloor, imgWallL, imgWallR; // 背景画像
int imgFighter, imgBullet; // 自機と自機の弾の画像
int imgEnemy[IMG_ENEMY_MAX]; // 敵機の画像
int imgExplosion; // 爆発演出の画像
int imgItem; // アイテムの画像
int distance = 0; // ステージ終端までの距離
int bossIdx = 0; // ボスを代入した配列のインデックス
int stage = 1; // ステージ
int score = 0; // スコア
int hisco = 10000; // ハイスコア
int noDamage = 0; // 無敵状態
int weaponLv = 1; // 自機の部位のレベル（同時に発射される弾数）
int scene = TITLE; // シーンを管理
int timer = 0; // 時間の進行を管理

struct OBJECT player; // 自機用の構造体変数
struct OBJECT bullet[BULLET_MAX]; // 弾用の構造体の配列
struct OBJECT enemy[ENEMY_MAX]; // 敵機用の構造体の配列
struct OBJECT effect[EFFECT_MAX]; // エフェクト用の構造体の配列
struct OBJECT item; // アイテム用の構造体変数

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetWindowText("シューティングゲーム"); // ウィンドウのタイトル
	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(TRUE); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化　エラーが起きたら終了
	SetBackgroundColor(0, 0, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏画面にする

	initGame(); // 初期化用の関数を呼び出す
	initVariable(); // 【仮】ゲームを完成させる際に呼び出し位置を変える

	distance = STAGE_DISTANCE; // 【記述位置は仮】ステージの長さを代入

	while (1) // メインループ
	{
		ClearDrawScreen(); //画面をクリアにする 

		// ゲームの骨組みとなる処理を、ここに記述する
		int spd = 1;; // スクロールの速さ
		if (scene == PLAY && distance == 0) spd = 0; // ボス戦はスクロール停止

		scrollBG(spd); // 背景のスクロール
		moveEnemy(); // 敵機の制御
		movePlayer(); // 自機の操作
		moveBullet(); // 弾の制御
		moveItem(); // アイテムの制御
		drawEffect(); // エフェクト
		stageMap(); // ステージマップ
		drawParameter(); // 自機のシールドなどのパラメーターを表示

		timer++;

		switch (scene)
		{

		case TITLE: // タイトル画面

			drawTextC(WIDTH * 0.5, HEIGHT * 0.3, "Shooting Game", 0xffffff, 80);
			drawTextC(WIDTH * 0.5, HEIGHT * 0.7, "Press SPACE to start.", 0xffffff, 30);

			if (CheckHitKey(KEY_INPUT_SPACE))
			{
				initVariable();
				scene = PLAY;
			}

			break;

		case PLAY: // ゲームプレイ画面

			movePlayer(); // 自機の操作

			if (distance == STAGE_DISTANCE)
			{
				srand(stage); // ステージのパターンを決める
			}

			if (distance > 0) distance--;

			if (300 < distance && distance % 20 == 0) // ザコ1と2の出現
			{
				int x = 100 + rand() % (WIDTH - 200);
				int y = -50;
				int e = 1 + rand() % 2;

				if (e == ENE_ZAKO1) setEnemy(x, y, 0, 3, ENE_ZAKO1, imgEnemy[ENE_ZAKO1], 1);

				if (e == ENE_ZAKO2)
				{
					int vx = 0;
					if (player.x < x - 50) vx = -3;
					if (player.x > x + 50) vx = 3;
					setEnemy(x, -100, vx, 5, ENE_ZAKO2, imgEnemy[ENE_ZAKO2], 3);
				}

			}
			if (300 < distance && distance < 900 && distance % 30 == 0) // ザコ３の出現
			{
				int x = 100 + rand() % (WIDTH - 200);
				int y = -50;
				int vy = 40 + rand() % 20;
				setEnemy(x, -100, 0, vy, ENE_ZAKO3, imgEnemy[ENE_ZAKO3], 5);
			}

			if (distance == 1) // ボス出現
			{
				bossIdx = setEnemy(WIDTH / 2, -120, 0, 1, ENE_BOSS, imgEnemy[ENE_BOSS], 200);
			}

			if (distance % 800 == 1) setItem(); // アイテムの出現

			if (player.shield == 0)
			{
				scene = OVER;
				timer = 0;
				break;
			}

			break;

		case OVER: // ゲームオーバー

			if (timer < FPS * 3) // 自機が爆発する演出
			{
				if (timer % 7 == 0)
				{
					setEffect(player.x + rand() % 81 - 40, player.y + rand() % 81 - 40, EFE_EXPLODE);
				}
			}
			else
			{
				drawTextC(WIDTH * 0.5, HEIGHT * 0.3, "GAME OVER", 0xff0000, 80);
			}

			if (timer > FPS * 10) scene = TITLE; // タイトルへ遷移

			break;

		case CLEAR: // ステージクリアー

			movePlayer(); // 自機の処理

			if (timer < FPS * 3) // ボスが爆発する演出
			{
				if (timer % 7 == 0)
				{
					setEffect(enemy[bossIdx].x + rand() % 201 - 100, enemy[bossIdx].y + rand() % 201 - 100, EFE_EXPLODE);
				}
			}
			else
			{
				drawTextC(WIDTH * 0.5, HEIGHT * 0.3, "STAGE CLEAR!", 0x00ffff, 80);
			}

			if (timer > FPS * 10) // ゲームプレイへ遷移
			{
				stage++;
				distance = STAGE_DISTANCE;
				scene = PLAY;
			}

			break;
		}

		// スコア、ハイスコア、ステージ数の表示
		drawText(10, 10, "SCORE %7d", score, 0xffffff, 30);
		drawText(WIDTH - 220, 10, "HI-SC %07d", hisco, 0xffffff, 30);
		drawText(WIDTH - 145, HEIGHT - 40, "STAGE %02d", stage, 0xffffff, 30);

		ScreenFlip(); // 裏画面の内容を表画面に反映させる
		WaitTimer(1000 / FPS); // 一定時間待つ
		if (ProcessMessage() == -1) break; // Windowsから情報を受け取り、エラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break; // ESCキーが押されたら終了
	}

	DxLib_End(); // DXライブラリ使用の終了処理
	return 0; // ソフトの終了
}

// ここから下に自作した関数を記述する
// 初期化用の関数
void initGame(void)
{
	// 背景用の画像の読み込み
	imgGalaxy = LoadGraph("image/bg0.png");
	imgFloor = LoadGraph("image/bg1.png");
	imgWallL = LoadGraph("image/bg2.png");
	imgWallR = LoadGraph("image/bg3.png");

	// 自機と自機の弾の読み込み
	imgFighter = LoadGraph("image/fighter.png");
	imgBullet = LoadGraph("image/bullet.png");

	// 敵機の画像の読み込み
	for (int i = 0; i < IMG_ENEMY_MAX;i++)
	{
		char file[] = "image/enemy*.png";
		file[11] = (char)('0' + i);
		imgEnemy[i] = LoadGraph(file);
	}

	// その他の画像の読み込み
	imgExplosion = LoadGraph("image/explosion.png"); // 爆発演出
	imgItem = LoadGraph("image/item.png"); // アイテム
}

// 背景のスクロール
void scrollBG(int spd)
{
	static int galaxyY, floorY, wallY; // スクロール位置を管理する変数（静的記憶領域に保持される）
	galaxyY = (galaxyY + spd) % HEIGHT; // 星空（宇宙）
	DrawGraph(0, galaxyY - HEIGHT, imgGalaxy, FALSE);
	DrawGraph(0, galaxyY, imgGalaxy, FALSE);
	floorY = (floorY + spd * 2) % 120; // 床

	for (int i = -1;i < 6;i++)
	{
		DrawGraph(240, floorY + i * 120, imgFloor, TRUE);
	}

	wallY = (wallY + spd * 4) % 240; // 左右の壁
	DrawGraph(0, wallY - 240, imgWallL, TRUE);
	DrawGraph(WIDTH - 300, wallY - 240, imgWallR, TRUE);

}

// ゲーム開始時の初期値を代入する関数
void initVariable(void)
{
	player.x = WIDTH / 2;
	player.y = HEIGHT / 2;
	player.vx = 5;
	player.vy = 5;
	player.shield = PLAYER_SHIELD_MAX;

	GetGraphSize(imgFighter, &player.wid, &player.hei); // 自機の画像の幅と高さを代入

	for (int i = 0; i < ENEMY_MAX; i++) enemy[i].state = 0; // すべての敵機を存在しない状態に

	score = 0;
	stage = 1;
	noDamage = 0;
	weaponLv = 1;
	distance = STAGE_DISTANCE;
}

// 中心座標を指定して画像を表示する関数
void drawImage(int img, int x, int y)
{
	int w, h;
	GetGraphSize(img, &w, &h);
	DrawGraph(x - w / 2, y - h / 2, img, TRUE);
}

// 自機を動かす関数
void movePlayer(void)
{
	static char oldSpcKey;
	static int countSpcKey;

	if (CheckHitKey(KEY_INPUT_UP)) // 上キー
	{
		player.y -= player.vy;
		if (player.y < 30) player.y = 30;
	}

	if (CheckHitKey(KEY_INPUT_DOWN)) // 下キー
	{
		player.y += player.vy;
		if (player.y > HEIGHT - 30) player.y = HEIGHT - 30;
	}

	if (CheckHitKey(KEY_INPUT_LEFT)) // 左キー
	{
		player.x -= player.vx;
		if (player.x < 30) player.x = 30;
	}

	if (CheckHitKey(KEY_INPUT_RIGHT)) // 右キー
	{
		player.x += player.vx;
		if (player.x > WIDTH - 30) player.x = WIDTH - 30;
	}

	if (CheckHitKey(KEY_INPUT_SPACE)) // スペースキー
	{
		if (oldSpcKey == 0)
		{
			setBullet(); // 押した瞬間発射
		}
		else if (countSpcKey % 10 == 0)
		{
			setBullet(); // 一定間隔で発射
		}
		countSpcKey++;
	}
	else
	{
		countSpcKey = 0;
	}

	oldSpcKey = CheckHitKey(KEY_INPUT_SPACE); // スペースキーの状態を保持

	if (noDamage > 0) noDamage--; // 無敵時間のカウント
	if (noDamage % 4 < 2) drawImage(imgFighter, player.x, player.y); // 自機の描画
}

// 弾のセット（発射）
void setBullet(void)
{
	for (int n = 0; n < weaponLv; n++)
	{
		int x = player.x - (weaponLv - 1) * 5 + n * 10;
		int y = player.y - 20;

		for (int i = 0;i < BULLET_MAX; i++)
		{
			if (bullet[i].state == 0) // 空いている配列に弾をセット
			{
				bullet[i].x = x;
				bullet[i].y = y;
				bullet[i].vx = 0;
				bullet[i].vy = -40; // y軸方向の速さ（１回の計算で移動するピクセル数）
				bullet[i].state = 1; // 弾を存在できるようにする

				break;
			}
		}
	}
}

// 弾の移動
void moveBullet(void)
{
	for (int i = 0;i < BULLET_MAX; i++)
	{
		if (bullet[i].state == 0) continue;

		bullet[i].x += bullet[i].vx; // 座標を変化させる
		bullet[i].y += bullet[i].vy; // 同上
		drawImage(imgBullet, bullet[i].x, bullet[i].y); // 弾の描画

		// 画面外に出たら存在しない状態にする
		if (bullet[i].y < -100)bullet[i].state = 0;
	}
}

// 敵機をセットする
int setEnemy(int x, int y, int vx, int vy, int ptn, int img, int sld)
{
	for (int i = 0;i < ENEMY_MAX; i++)
	{
		if (enemy[i].state == 0)
		{
			enemy[i].x = x;
			enemy[i].y = y;
			enemy[i].vx = vx;
			enemy[i].vy = vy;
			enemy[i].state = 1;
			enemy[i].pattern = ptn;
			enemy[i].image = img;
			enemy[i].shield = sld * stage; // ステージが進むほど敵が固くなる

			GetGraphSize(img, &enemy[i].wid, &enemy[i].hei); // 画像の幅と高さを代入

			return i;
		}
	}
	return -1;
}

// 敵機を動かす
void moveEnemy(void)
{
	for (int i = 0;i < ENEMY_MAX; i++)
	{
		if (enemy[i].state == 0) continue; // 空いている配列なら処理しない

		if (enemy[i].pattern == ENE_ZAKO3) // ザコ機３
		{
			if (enemy[i].vy > 1) // 減速
			{
				enemy[i].vy *= 0.9;
			}
			else if (enemy[i].vy > 0) // 弾発射、飛び去る
			{
				setEnemy(enemy[i].x, enemy[i].y, 0, 6, ENE_BULLET, imgEnemy[ENE_BULLET], 0); // 弾

				enemy[i].vx = 8;
				enemy[i].vy = -4;
			}
		}

		if (enemy[i].pattern == ENE_BOSS) // ボス機
		{
			if (enemy[i].y > HEIGHT - 120) enemy[i].vy = -4;

			if (enemy[i].y < 120) // 画面上端
			{
				if (enemy[i].vy < 0) // 弾発射
				{
					for (int bx = -2;bx <= 2; bx++) // 二重ループのfor
					{
						for (int by = 0; by <= 3; by++)
						{
							if (bx == 0 && by == 0) continue;

							setEnemy(enemy[i].x, enemy[i].y, bx * 2, by * 3, ENE_BULLET, imgEnemy[ENE_BULLET], 0);
						}
					}
				}
				enemy[i].vy = 4;
			}
		}

		enemy[i].x += enemy[i].vx; // 敵機の移動
		enemy[i].y += enemy[i].vy; // 同上
		drawImage(enemy[i].image, enemy[i].x, enemy[i].y); // 敵機の描画

		// 画面外に出たか
		if (enemy[i].x < -200 || WIDTH + 200 < enemy[i].x || enemy[i].y < -200 || HEIGHT + 200 < enemy[i].y) enemy[i].state = 0;

		// 当たり判定のアルゴリズム
		if (enemy[i].shield > 0) // ヒットチェックを行う敵機（弾以外）
		{
			for (int j = 0; j < BULLET_MAX; j++) // 自機の弾とヒットチェック
			{
				if (bullet[j].state == 0) continue;

				int dx = abs((int)(enemy[i].x - bullet[j].x)); // 中心座標間のピクセル数
				int dy = abs((int)(enemy[i].y - bullet[j].y)); // 同上

				if (dx < enemy[i].wid / 2 && dy < enemy[i].hei / 2) // 接触しているか
				{
					bullet[j].state = 0; // 弾を消す
					damageEnemy(i, 1); // 敵にダメージ
				}
			}
		}

		if (noDamage == 0) // 無敵状態でないとき、自機とヒットチェック
		{
			int dx = abs(enemy[i].x - player.x); // 中心座標間のピクセル数
			int dy = abs(enemy[i].y - player.y); // 同上

			if (dx < enemy[i].wid / 2 + player.wid / 2 && dy < enemy[i].hei / 2 + player.hei / 2)
			{
				if (player.shield > 0) player.shield--; // シールドを減らす

				noDamage = FPS; // 無敵状態をセット
				damageEnemy(i, 1); // 敵にダメージ
			}
		}
	}
}

// ステージマップ
void stageMap(void)
{
	int mx = WIDTH - 30, my = 60; // マップの表示位置
	int wi = 20, he = HEIGHT - 120; // マップの幅、高さ
	int pos = (HEIGHT - 140) * distance / STAGE_DISTANCE; // 自機の飛行している位置

	SetDrawBlendMode(DX_BLENDMODE_SUB, 128); // 減算による描画の重ね合わせ
	DrawBox(mx, my, mx + wi, my + he, 0xffffff, TRUE);

	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // ブレンドモードを解除
	DrawBox(mx - 1, my - 1, mx + wi + 1, my + he + 1, 0xffffff, FALSE); // 枠線
	DrawBox(mx, my + pos, mx + wi, my + pos + 20, 0x0080ff, TRUE); // 自機の位置
}

// 敵機のシールドを減らす
void damageEnemy(int n, int dmg)
{
	SetDrawBlendMode(DX_BLENDMODE_ADD, 192); // 加算による描画の重ね合わせ
	DrawCircle(enemy[n].x, enemy[n].y, (enemy[n].wid + enemy[n].hei) / 4, 0xff0000, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // ブレンドモードの解除

	score += 100; // スコアの加算

	if (score > hisco) hisco = score; // ハイスコアの更新

	enemy[n].shield -= dmg; // シールドを減らす

	if (enemy[n].shield <= 0)
	{
		enemy[n].state = 0; // シールド０以下で消す
		setEffect(enemy[n].x, enemy[n].y, EFE_EXPLODE); // 爆発演出

		if (enemy[n].pattern == ENE_BOSS) // ボスを倒した
		{
			scene = CLEAR;
			timer = 0;
		}
	}
}

// 影を付けた文字列と値を表示する関数
void drawText(int x, int y, const char* txt, int val, int col, int siz)
{
	SetFontSize(siz); // フォントの大きさを指定

	DrawFormatString(x + 1, y + 1, 0x000000, txt, val); // 黒で文字列を表示
	DrawFormatString(x, y, col, txt, val); // 引数の色で文字列を表示
}

// 自機に関するパラメーターを表示
void drawParameter(void)
{
	int x = 10, y = HEIGHT - 30; // 表示位置

	DrawBox(x, y, x + PLAYER_SHIELD_MAX * 30, y + 20, 0x000000, TRUE);

	for (int i = 0; i < player.shield; i++) // シールドのメーター
	{
		int r = 128 * (PLAYER_SHIELD_MAX - 1) / PLAYER_SHIELD_MAX; // RGBを計算
		int g = 255 * i / PLAYER_SHIELD_MAX;
		int b = 160 + 96 * i / PLAYER_SHIELD_MAX;

		DrawBox(x + 2 + i * 30, y + 2, x + 28 + i * 30, y + 18, GetColor(r, g, b), TRUE);
	}

	drawText(x, y - 25, "SHIELD Lv %02d", player.shield, 0xffffff, 20); // シールド値
	drawText(x, y - 50, "WEAPON LV %02d", weaponLv, 0xffffff, 20); // 武器レベル
	drawText(x, y - 75, "SPEED %02d", player.vx, 0xffffff, 20); // 移動速度
}

// エフェクトのセット
void setEffect(int x, int y, int ptn)
{
	static int eff_num;

	effect[eff_num].x = x;
	effect[eff_num].y = y;
	effect[eff_num].state = 1;
	effect[eff_num].pattern = ptn;
	effect[eff_num].timer = 0;

	eff_num = (eff_num + 1) % EFFECT_MAX;
}

// エフェクトの描画
void drawEffect(void)
{
	int ix;

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].state == 0)continue;

		switch (effect[i].pattern) // エフェクトごとに処理を分ける
		{
		case EFE_EXPLODE: // 爆発演出
			ix = effect[i].timer * 128; // 画像の切り出し位置

			DrawRectGraph(effect[i].x - 64, effect[i].y - 64, ix, 0, 128, 128, imgExplosion, TRUE, FALSE);

			effect[i].timer++;

			if (effect[i].timer == 7)effect[i].state = 0;

			break;

		case EFE_RECOVER: // 回復演出

			if (effect[i].timer < 30) // 加算による描画の重ね合わせ
			{
				SetDrawBlendMode(DX_BLENDMODE_ADD, effect[i].timer * 8);
			}
			else
			{
				SetDrawBlendMode(DX_BLENDMODE_ADD, (60 - effect[i].timer) * 8);
			}

			for (int i = 3; i < 8; i++)
			{
				DrawCircle(player.x, player.y, (player.wid + player.hei) / i, 0x2040c0, TRUE);
			}

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); // ブレンドモードを解除

			effect[i].timer++;

			if (effect[i].timer == 60) effect[i].state = 0;

			break;
		}
	}
}

// アイテムをセット
void setItem(void)
{
	item.x = (WIDTH / 4) * (1 + rand() % 3);
	item.y = -16;
	item.vx = 15;
	item.vy = 1;
	item.state = 1;
	item.timer = 0;
}

// アイテムの処理
void moveItem(void)
{
	if (item.state == 0) return;

	item.x += item.vx;
	item.y += item.vy;

	if (item.timer % 60 < 30) item.vx -= 1;
	else item.vx += 1;

	if (item.y > HEIGHT + 16) item.state = 0;

	item.pattern = (item.timer / 120) % ITEM_TYPE; // 現在、どのアイテムになっているか
	item.timer++;

	DrawRectGraph(item.x - 20, item.y - 16, item.pattern * 40, 0, 40, 32, imgItem, TRUE, FALSE);
	if (scene == OVER) return; // ゲームオーバー画面では回収できない

	int dis = (item.x - player.x) * (item.x - player.x) + (item.y - player.y) * (item.y - player.y);

	if (dis < 60 * 60)
	{
		item.state = 0;

		if (item.pattern == 0) // スピードアップ
		{
			if (player.vx < PLAYER_SPEED_MAX)
			{
				player.vx += 3;
				player.vy += 3;
			}
		}

		if (item.pattern == 1) // シールド回復
		{
			if (player.shield < PLAYER_SHIELD_MAX) player.shield++;
			setEffect(player.x, player.y, EFE_RECOVER); // 回復エフェクトを表示
		}

		if (item.pattern == 2) // 武器レベルアップ
		{
			if (weaponLv < WEAPON_LV_MAX) weaponLv++;
		}
	}
}

// 文字をセンタリングして表示する関数
void drawTextC(int x, int y, const char* txt, int col, int siz)
{
	SetFontSize(siz);

	int strWidth = GetDrawStringWidth(txt, strlen(txt));

	x -= strWidth / 2;
	y -= siz / 2;

	DrawString(x + 1, y + 1, txt, 0x000000);
	DrawString(x, y, txt, col);
}