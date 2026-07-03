#include "Application.h"
#include <DxLib.h>
#include "../General/game.h"
#include "../General/ShaderPostProcess.h"
#include "../General/Input.h"
#include "../General/Collision/Physics.h"
#include "../Scene/SceneController.h"
#include "../General/Fader.h"
#include "../General/Effect/EffekseerManager.h"
#include "../General/AssetManager.h"
#include "../Game/UI/UIManager.h"
#include "../General/Sound/SoundManager.h"
#include "../General/LoadingManager.h"
#include "../General/SaveDataManager.h"
#include "../General/CSV/OptionData.h"
#include "../resource.h"
#include  <cassert>
#include <chrono>
namespace
{
	//ターゲットFPS
	const int kTargetFPS = 60;
	const int kWarningFPS = 40;
	//ミリ秒
	float kMillisecond = 1000.0f;
	//マイクロ秒
	float kMicrosecond = 1000000.0f;
	//タイムスケール
	float kTimeScale = 1.0f;
}

Application& Application::GetInstance()
{
    //この時点でメモリが確保されてアプリ終了まで残る
    static Application app;
    return app;
}

bool Application::Init()
{
	//初期化
	//このフラグがtrueの時アプリケーションが終了する
	m_isFinishApplication = false;

	//ゲームタイトル
	SetWindowText(L"Project:NeaR");

	//ゲームアイコン
	SetWindowIconID(IDI_ICON1);

	//セーブデータ
	auto& saveDataManager = SaveDataManager::GetInstance();
	saveDataManager.LoadSaveData();
	//ウィンドウモード
	m_isWindow = saveDataManager.GetOptionData()->IsWindow();

	 //フルスクリーンでなく、ウィンドウモードで開くようにする
	//こういった関数はウィンドウが開く前に(Dxlib.Init()の前)に処理しておく必要がある
	ChangeWindowMode(m_isWindow);

	// DirectX11を使用するようにする。(DirectX9も可、一部機能不可)
	// Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	//画面サイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);


	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return false;			// エラーが起きたら直ちに終了
	}

	//描画先を裏画面にする
	SetDrawScreen(DX_SCREEN_BACK);
	SetDrawMode(DX_DRAWMODE_NEAREST);

	//3D描画のための準備
	//Zバッファを使用する
	SetUseZBuffer3D(true);
	//Zバッファに書き込みを行う
	SetWriteZBuffer3D(true);
	//バックカリングを行う(ポリゴンの裏面を描画しないようにする)
	SetUseBackCulling(true);

	//エフェクト関連
	EffekseerManager::GetInstance().InitEffekseer();

	//変数の初期化
	m_timeScale = kTimeScale;
	m_isUpdateStop = false;

	//ロードマネージャー
	auto& loadingManager = LoadingManager::GetInstance();
	loadingManager.Init();

    return true;
}

void Application::Run()
{
	//ロードマネージャー
	auto& loadingManager = LoadingManager::GetInstance();

	//アセットマネージャー
	auto& assetManager = AssetManager::GetInstance();
	assetManager.Init();

	//ポストエフェクトの準備
	m_postProcess = std::make_unique<ShaderPostProcess>();
	m_postProcess->Init();

	//コントローラー
	auto& input = Input::GetInstance();
	input.Init();

	//Physics(衝突処理)
	auto& physics = Physics::GetInstance();
	physics.Init();

	//フェード
	auto& fader = Fader::GetInstance();
	fader.Init();

	//UI
	auto& uiManager = UIManager::GetInstance();

	//音
	auto& soundManager = SoundManager::GetInstance();
	soundManager.Init();

	//シーン
	std::unique_ptr<SceneController> sceneController = std::make_unique<SceneController>();

	//レンダーターゲット(Drawの書き込み先)
	auto RT = MakeScreen(Game::kScreenWidth, Game::kScreenHeight);

	//ゲームループ
	while (ProcessMessage() != -1) // Windowsが行う処理を待つ
	{
		//今回のループが始まった時間を覚えておく
		LONGLONG time = GetNowHiPerformanceCount();

		//更新
		input.Update();

		//Windowモードが切り替わったかをチェック
		bool isWindow = m_isWindow;

#if _DEBUG
		if (input.IsTrigger("Glitch"))
		{
			m_postProcess->AddPostEffectState(ShaderPostProcess::PostEffectState::Glitch);
		}
		if (input.IsTrigger("GlitchSub"))
		{
			m_postProcess->SubPostEffectState(ShaderPostProcess::PostEffectState::Glitch);
		}
		if (input.IsTrigger("Gray"))
		{
			m_postProcess->AddPostEffectState(ShaderPostProcess::PostEffectState::Gray);
		}
		if (input.IsTrigger("GraySub"))
		{
			m_postProcess->SubPostEffectState(ShaderPostProcess::PostEffectState::Gray);
		}

		
		if (input.IsTrigger("StopUpdate"))
		{
			m_isUpdateStop = !m_isUpdateStop;
		}
		if (!m_isUpdateStop || input.IsTrigger("OneFrame"))
#endif
		{
			//ターゲット
			SetDrawScreen(RT);

			//画面全体をクリア
			ClearDrawScreen();

			//非同期ロード中でなければ更新
			if (!loadingManager.IsLoading())
			{
				//シーン
				sceneController->Update();
			}

			//ロードチェック更新
			assetManager.Update();

			//非同期ロード中でなければ更新
			if (!loadingManager.IsLoading())
			{
				//フェード
				fader.Update();
				//当たり判定処理
				physics.Update();
				//シェーダー
				m_postProcess->Update();
				//UIマネージャー
				uiManager.Update();
				//音
				soundManager.Update();
			}
		}
		
		//非同期ロード中でなければ描画
		if (!loadingManager.IsLoading())
		{
			//描画
			sceneController->Draw();

			//前描画
			uiManager.FrontDraw();
		}
		

		//裏描画
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
	
		if (!loadingManager.IsLoading())
		{
			//裏画面にレンダーターゲットを描画
			m_postProcess->Draw(RT);
			//後描画
			uiManager.BackDraw();
		}
		//フェード
		fader.Draw();

		//非同期ロード中の処理
		loadingManager.Update();
		loadingManager.Draw();

#if _DEBUG

		if (input.IsPress("AddTimeScale"))
		{
			m_timeScale += 0.01f;
		}
		else if (input.IsPress("SubTimeScale"))
		{
			m_timeScale -= 0.01f;
		}
		m_timeScale = MathSub::ClampFloat(m_timeScale, 0.0f, 5.0f);

		//状態切り替え
		if(input.IsTrigger("Select"))
		{
			m_debugState = static_cast<DebugState>((static_cast<int>(m_debugState) + 1) % static_cast<int>(DebugState::DebugMax));
		}
		switch (m_debugState)
		{
		case Application::DebugState::FPS:
			// デバッグ表示
			DebugDrawFPS();
			break;
		case Application::DebugState::SceneInfo:
			sceneController->DebugDraw();
			break;
		case Application::DebugState::DebugMax:
			break;
		default:
			break;
		}

		auto fps = DxLib::GetFPS();
		//もしもFPSが低下したら
		if (fps <= kWarningFPS)
		{
			SYSTEMTIME lt;
			GetLocalTime(&lt);
			printf("FPS Low! fps = %.2f : Time = %02d:%02d\n", fps, lt.wHour, lt.wMinute);
			
		}

		DrawFormatString(10, 10, 0xff0000, L"LoadingNum : %d",GetASyncLoadNum());
#endif

		//切り替わったなら
		if (m_isWindow != isWindow)
		{
			//切り替わり処理
			ChangeScreenMode();
			DeleteGraph(RT);
			RT = MakeScreen(Game::kScreenWidth, Game::kScreenHeight);
		}

		//画面の切り替わりを待つ必要がある
		ScreenFlip();



		//FPSを60に固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{

		}

		//ESCキーで終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) || m_isFinishApplication)
		{
			break;
		}
	}
}

void Application::Terminate()
{
	//ポストエフェクトの終了
	m_postProcess->End();

	//エフェクト関連
	EffekseerManager::GetInstance().EndEffekseer();
	
	//当たり判定の終了
	Physics::GetInstance().Reset();

	//音
	SoundManager::GetInstance().End();

	//アセットの削除
	AssetManager::GetInstance().End();

	//ロードの削除
	LoadingManager::GetInstance().End();

	//ＤＸライブラリ使用の終了処理
	DxLib_End();				
}

void Application::SetWindowMode(bool isWindow)
{
	m_isWindow = isWindow;
}

void Application::ChangeScreenMode()
{
	//画面モード変更時(とウインドウモード変更時)にグラフィックスシステムの設定やグラフィックハンドルをリセットするかどうかを設定する
	//Flag TRUE:リセットする(デフォルト)FALSE:リセットしない
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	//解像度
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);
	//切り替え
	ChangeWindowMode(m_isWindow);
	//実際のウィンドウサイズも設定（フレーム込み）
	SetWindowSize(Game::kScreenWidth, Game::kScreenHeight);
	//て拡大率を1倍に戻す
	SetWindowSizeExtendRate(1.0);
	//画面全体をクリア
	ClearDrawScreen();
}

void Application::DebugDrawFPS() const
{
	//デバッグ表示
	DrawFormatString(10, 10, 0xffffff, L"fps=%.2f", DxLib::GetFPS());
	DrawFormatString(10, 26, 0xffffff, L"TimeScale=%.2f", m_timeScale);
}

