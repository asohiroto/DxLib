#include "DxLib.h"
#include <stdlib.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	const int WIDTH = 960, HEIGHT = 640;

	SetWindowText("テニスゲーム");
	SetGraphMode(WIDTH, HEIGHT, 32);
	ChangeWindowMode(TRUE);
	if (DxLib_Init() == -1) return -1;
	SetBackgroundColor(0, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	int ballX = 40;
	int ballY = 80;
	int ballVX = 5;
	int ballVY = 5;
	int ballR = 10;

	int racketX = WIDTH / 2;
	int racketY = HEIGHT - 50;
	int racketW = 120;
	int racketH = 12;

	enum { TITLE, PLAY, OVER };
	int scene = TITLE;
	int timer = 0;
	int score = 0;
	int highScore = 1000;
	int dx, dy;

	while (1)
	{
		ClearDrawScreen();
		timer++;

		switch (scene)
		{
		case TITLE:
			SetFontSize(50);
			DrawString(WIDTH / 2 - 50 / 2 * 12 / 2, HEIGHT / 3, "Tennis Game", 0x00ff00);
			if (timer % 60 < 30) {
				SetFontSize(30);
				DrawString(WIDTH / 2 - 30 / 2 * 21 / 2, HEIGHT * 2 / 3, "Press SPACE to Start", 0x00ffff);
			}

			if (CheckHitKey(KEY_INPUT_SPACE) == 1)
			{
				ballX = 40;
				ballY = 80;
				ballVX = 5;
				ballVY = 5;
				racketX = WIDTH / 2;
				racketY = HEIGHT - 50;
				score = 0;
				scene = PLAY;
			}
			break;

		case PLAY:

			ballX += ballVX;
			if (ballX < ballR && ballVX < 0) ballVX = -ballVX;
			if (ballX > WIDTH - ballR && ballVX > 0)ballVX = -ballVX;
			ballY += ballVY;
			if (ballY < ballR && ballVY < 0) ballVY = -ballVY;
			if (ballY > HEIGHT)
			{
				scene = OVER;
				timer = 0;
				break;
			}
			DrawCircle(ballX, ballY, ballR, 0xff0000, TRUE);

			if (CheckHitKey(KEY_INPUT_LEFT) == 1)
			{
				racketX -= 10;
				if (racketX < racketW / 2)racketX = racketW / 2;
			}
			if (CheckHitKey(KEY_INPUT_RIGHT) == 1)
			{
				racketX += 10;
				if (racketX > WIDTH - racketW / 2)racketX = WIDTH - racketW / 2;
			}

			DrawBox(racketX - racketW / 2, racketY - racketH / 2, racketX + racketW / 2, racketY + racketH / 2, 0x0080ff, TRUE);
		
			dx = ballX - racketX;
			dy = ballY - racketY;
			if (-racketW / 2 - 10 < dx && dx < racketW / 2 + 10 && -20 < dy && dy < 0)
			{
				ballVY = -5 - rand() % 5;
				score += 100;
				if (score > highScore)highScore = score;
			}
			break;
		
		case OVER:
			SetFontSize(40);
			DrawString(WIDTH / 2 - 40 / 2 * 9 / 2, HEIGHT / 3, "GAME OVER", 0xff0000);
			if (timer > 60 * 5) scene = TITLE;
			break;
		}



		SetFontSize(30);
		DrawFormatString(10, 10, 0xffff, "SCORE %d", score);
		DrawFormatString(WIDTH - 200, 10, 0xffff00, "HI-SC %d", highScore);

		ScreenFlip();
		WaitTimer(16);
		if (ProcessMessage() == -1) break;
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break;
	}

	DxLib_End();
	return 0;
}